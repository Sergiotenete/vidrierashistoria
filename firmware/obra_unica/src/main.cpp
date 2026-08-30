/*
 * Vidrieras Historia — instalación de una sola obra
 *
 * 100 LED WS2812B sobre ESP32, gobernados por un conmutador on/off:
 *   OFF -> ON : fundido de entrada + efecto arcoíris en movimiento
 *   ON  -> OFF: fundido de salida hasta negro
 *
 * El conmutador NO corta la corriente: es una señal que lee el ESP32. Por eso
 * el fundido de salida puede llegar a completarse. Ver docs/01-instalacion-electrica.md.
 *
 * Todos los ajustes están en include/config.h.
 */

#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>
#include <esp_task_wdt.h>

#include "config.h"

#if MODO_REPOSO != 0
  #include <esp_sleep.h>
  #include <driver/rtc_io.h>
  #include <driver/gpio.h>
#endif

#if MODO_REPOSO == 2
  #if !(PIN_CONMUTADOR == 0 || PIN_CONMUTADOR == 2 || PIN_CONMUTADOR == 4 || \
        (PIN_CONMUTADOR >= 12 && PIN_CONMUTADOR <= 15) ||                    \
        (PIN_CONMUTADOR >= 25 && PIN_CONMUTADOR <= 27) ||                    \
        (PIN_CONMUTADOR >= 32 && PIN_CONMUTADOR <= 39))
    #error "Con MODO_REPOSO 2, PIN_CONMUTADOR debe ser un GPIO del dominio RTC (0, 2, 4, 12-15, 25-27, 32-39)."
  #endif
  #if USAR_CORTE_ALIMENTACION
    #if !(PIN_ALIM_LEDS == 0 || PIN_ALIM_LEDS == 2 || PIN_ALIM_LEDS == 4 || \
          (PIN_ALIM_LEDS >= 12 && PIN_ALIM_LEDS <= 15) ||                   \
          (PIN_ALIM_LEDS >= 25 && PIN_ALIM_LEDS <= 27) ||                   \
          (PIN_ALIM_LEDS >= 32 && PIN_ALIM_LEDS <= 33))
      #error "Con MODO_REPOSO 2 y corte de alimentacion, PIN_ALIM_LEDS debe ser un GPIO RTC de salida (0, 2, 4, 12-15, 25-27, 32-33)."
    #endif
  #endif
#endif

static CRGB leds[NUM_LEDS];

enum Estado : uint8_t { APAGADO, ENCENDIENDO, ENCENDIDO, APAGANDO };
static Estado estado = APAGADO;

// Envolvente del fundido, en escala perceptual 0..255.
static uint8_t  nivel        = 0;
static uint8_t  nivelInicio  = 0;
static uint8_t  nivelDestino = 0;
static uint32_t tInicio      = 0;
static uint32_t duracion     = 0;

static uint16_t hue16   = 0;   // tono en 1/256 de unidad, para un avance suave
static uint32_t tFrame  = 0;
static bool     railVivo = true;
static uint32_t tApagado = 0;   // instante en que la obra quedó apagada

static const uint32_t INTERVALO_FRAME_MS = 1000 / FPS;

// ---------------------------------------------------------------------------
// Alimentación de la tira (opcional)
// ---------------------------------------------------------------------------
static void railLeds(bool encender) {
#if USAR_CORTE_ALIMENTACION
  if (encender == railVivo) return;
  if (encender) {
    pinMode(PIN_DATOS, OUTPUT);
    digitalWrite(PIN_ALIM_LEDS, ALIM_ACTIVA_EN);
    railVivo = true;
  } else {
    digitalWrite(PIN_ALIM_LEDS, ALIM_ACTIVA_EN == HIGH ? LOW : HIGH);
    // Pin de datos en alta impedancia: sin tensión en la tira, un pin de datos
    // activo inyectaría corriente por los diodos de protección del primer LED.
    pinMode(PIN_DATOS, INPUT);
    railVivo = false;
  }
#else
  (void)encender;
  railVivo = true;
#endif
}

// ---------------------------------------------------------------------------
// Lectura del conmutador
// ---------------------------------------------------------------------------
static bool contactoEstable() {
  static bool     estable   = false;
  static bool     ultima    = false;
  static uint32_t tCambio   = 0;

  bool lectura = (digitalRead(PIN_CONMUTADOR) == LOW);   // cerrado a GND
#if CONMUTADOR_INVERTIDO
  lectura = !lectura;
#endif

  if (lectura != ultima) {
    ultima  = lectura;
    tCambio = millis();
  } else if ((uint32_t)(millis() - tCambio) >= ANTIRREBOTE_MS) {
    estable = lectura;
  }
  return estable;
}

static bool seQuiereEncendida() {
#if MODO_CONMUTADOR == 0
  // Interruptor enclavado: su posición es directamente el estado deseado.
  return contactoEstable();
#else
  // Pulsador momentáneo: cada flanco de pulsación alterna el estado.
  static bool anterior = false;
  static bool deseo    = false;
  bool ahora = contactoEstable();
  if (ahora && !anterior) deseo = !deseo;
  anterior = ahora;
  return deseo;
#endif
}

// ---------------------------------------------------------------------------
// Fundidos
// ---------------------------------------------------------------------------
// Arranca una transición hacia `destino`. La duración se escala según lo que
// falte por recorrer, para que interrumpir un fundido a media altura no dé un
// salto de brillo ni tarde de más.
static void iniciarTransicion(uint8_t destino, uint32_t msRecorridoCompleto) {
  nivelInicio  = nivel;
  nivelDestino = destino;
  uint16_t recorrido = (nivelInicio > destino) ? (uint16_t)(nivelInicio - destino)
                                               : (uint16_t)(destino - nivelInicio);
  duracion = (uint32_t)msRecorridoCompleto * recorrido / 255UL;
  tInicio  = millis();
}

// Devuelve true cuando la transición ha terminado.
static bool avanzarTransicion() {
  if (duracion == 0) { nivel = nivelDestino; return true; }
  uint32_t t = millis() - tInicio;
  if (t >= duracion) { nivel = nivelDestino; return true; }
  uint8_t p = (uint8_t)((t * 255UL) / duracion);
  nivel = lerp8by8(nivelInicio, nivelDestino, ease8InOutCubic(p));
  return false;
}

static void aplicarBrillo() {
#if APLICAR_GAMMA_FADE
  uint8_t lineal = dim8_raw(nivel);
#else
  uint8_t lineal = nivel;
#endif
  FastLED.setBrightness(scale8(BRILLO_MAXIMO, lineal));
}

// ---------------------------------------------------------------------------
// Efecto arcoíris
// ---------------------------------------------------------------------------
static void pintarArcoiris() {
  uint8_t base = (uint8_t)(hue16 >> 8);
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV((uint8_t)(base + i * EXTENSION_ARCOIRIS), SATURACION, 255);
  }
}

// ---------------------------------------------------------------------------
// Reposo del ESP32
// ---------------------------------------------------------------------------
#if MODO_REPOSO != 0
// Nivel del pin del conmutador que significa "enciende la obra". Es el que
// tiene que sacar al ESP32 del reposo.
  #if CONMUTADOR_INVERTIDO
    static const int NIVEL_DESPERTAR = HIGH;
  #else
    static const int NIVEL_DESPERTAR = LOW;
  #endif

static void entrarEnReposo() {
  // Si el conmutador ya está en el nivel que despierta, dormir sería inútil:
  // el ESP32 volvería en el acto. Pasa con un pulsador aún apretado.
  if (digitalRead(PIN_CONMUTADOR) == NIVEL_DESPERTAR) return;

  Serial.println(F("[obra] entrando en reposo"));
  Serial.flush();

  #if MODO_REPOSO == 1
    // Light sleep: la ejecución se detiene aquí y continúa en la línea
    // siguiente cuando el conmutador cambia de nivel.
    esp_task_wdt_delete(NULL);          // dormido no se puede alimentar
    gpio_wakeup_enable((gpio_num_t)PIN_CONMUTADOR,
                       NIVEL_DESPERTAR == LOW ? GPIO_INTR_LOW_LEVEL
                                              : GPIO_INTR_HIGH_LEVEL);
    esp_sleep_enable_gpio_wakeup();
    esp_light_sleep_start();
    gpio_wakeup_disable((gpio_num_t)PIN_CONMUTADOR);
    esp_task_wdt_add(NULL);
    tApagado = millis();                // margen antes de volver a dormirse
    Serial.println(F("[obra] despierta"));
  #else
    // Deep sleep: no retorna. La placa rearranca desde setup() al despertar.
    #if USAR_CORTE_ALIMENTACION
      // Mantiene el relé/MOSFET desactivado mientras el chip duerme; si no,
      // el pin queda flotante y el corte de la tira es impredecible.
      gpio_hold_en((gpio_num_t)PIN_ALIM_LEDS);
      gpio_deep_sleep_hold_en();
    #endif
    rtc_gpio_pullup_en((gpio_num_t)PIN_CONMUTADOR);
    rtc_gpio_pulldown_dis((gpio_num_t)PIN_CONMUTADOR);
    esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_CONMUTADOR,
                                 NIVEL_DESPERTAR == HIGH ? 1 : 0);
    esp_deep_sleep_start();
  #endif
}
#endif

// ---------------------------------------------------------------------------
// Watchdog
// ---------------------------------------------------------------------------
static void iniciarWatchdog() {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  esp_task_wdt_config_t cfg = {
    .timeout_ms     = WDT_SEGUNDOS * 1000,
    .idle_core_mask = 0,
    .trigger_panic  = true,
  };
  // El core 3.x puede haber inicializado ya el watchdog de tareas.
  if (esp_task_wdt_init(&cfg) == ESP_ERR_INVALID_STATE) esp_task_wdt_reconfigure(&cfg);
#else
  esp_task_wdt_init(WDT_SEGUNDOS, true);
#endif
  esp_task_wdt_add(NULL);   // vigila la tarea del loop()
}

// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println(F("\n[obra] Vidrieras Historia — obra única, 100 x WS2812B"));

#if DESACTIVAR_RADIO
  WiFi.mode(WIFI_OFF);
  #if defined(CONFIG_BT_ENABLED)
    btStop();
  #endif
#endif

#if FRECUENCIA_CPU_MHZ > 0
  setCpuFrequencyMhz(FRECUENCIA_CPU_MHZ);
#endif

  pinMode(PIN_CONMUTADOR, INPUT_PULLUP);

#if USAR_CORTE_ALIMENTACION
  #if MODO_REPOSO == 2
    // Tras un deep sleep el pin sigue retenido: hay que soltarlo para poder
    // volver a gobernarlo.
    gpio_deep_sleep_hold_dis();
    gpio_hold_dis((gpio_num_t)PIN_ALIM_LEDS);
  #endif
  pinMode(PIN_ALIM_LEDS, OUTPUT);
  digitalWrite(PIN_ALIM_LEDS, ALIM_ACTIVA_EN == HIGH ? LOW : HIGH);
  railVivo = false;
#endif

  FastLED.addLeds<TIPO_LED, PIN_DATOS, ORDEN_COLOR>(leds, NUM_LEDS)
         .setCorrection(TypicalLEDStrip);
  FastLED.setMaxPowerInVoltsAndMilliamps(VOLTIOS, MAX_MILIAMPERIOS);
  FastLED.setDither(BINARY_DITHER);
  FastLED.setBrightness(0);
  fill_solid(leds, NUM_LEDS, CRGB::Black);

  // La fuente necesita un instante para estabilizarse tras dar tensión.
  delay(ESPERA_ARRANQUE_MS);
  if (railVivo) FastLED.show();

  iniciarWatchdog();
  tApagado = millis();

  // Si al arrancar (por ejemplo, tras un corte de luz) el conmutador ya está
  // en ON, la obra entra con su fundido normal en lugar de golpe.
  Serial.println(F("[obra] lista"));
}

// ---------------------------------------------------------------------------
void loop() {
  esp_task_wdt_reset();

  const bool quiere = seQuiereEncendida();

  if (quiere && (estado == APAGADO || estado == APAGANDO)) {
#if USAR_CORTE_ALIMENTACION
    if (estado == APAGADO) { railLeds(true); delay(RETARDO_ALIM_MS); }
#endif
    iniciarTransicion(255, FADE_IN_MS);
    estado = ENCENDIENDO;
    Serial.println(F("[obra] fundido de entrada"));
  } else if (!quiere && (estado == ENCENDIDO || estado == ENCENDIENDO)) {
    iniciarTransicion(0, FADE_OUT_MS);
    estado = APAGANDO;
    Serial.println(F("[obra] fundido de salida"));
  }

  const uint32_t ahora = millis();
  if ((uint32_t)(ahora - tFrame) < INTERVALO_FRAME_MS) return;
  tFrame = ahora;

  if (estado == ENCENDIENDO) {
    if (avanzarTransicion()) {
      estado = ENCENDIDO;
      Serial.println(F("[obra] encendida"));
    }
  } else if (estado == APAGANDO) {
    if (avanzarTransicion()) {
      estado = APAGADO;
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      FastLED.setBrightness(0);
      FastLED.show();
      railLeds(false);
      tApagado = millis();
      Serial.println(F("[obra] apagada"));
      return;
    }
  } else if (estado == APAGADO) {
#if MODO_REPOSO != 0
    if (!quiere && (uint32_t)(ahora - tApagado) >= RETARDO_REPOSO_MS) entrarEnReposo();
#endif
    return;                       // nada que refrescar
  }

  hue16 += VELOCIDAD_ARCOIRIS;
  pintarArcoiris();
  aplicarBrillo();
  FastLED.show();
}
