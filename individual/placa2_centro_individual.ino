#include <NeoPixelBus.h>

// ============================================================================
// PLACA 2 - CENTRO (VERSION INDIVIDUAL / AUTONOMA)
// ----------------------------------------------------------------------------
// Version standalone basada en centro-v2-funciona, sin WiFi ni ESP-NOW.
// No depende de las placas Izquierda/Derecha: al tocar el sensor, esta
// placa ejecuta su propia animacion de inmediato, sin esperar ni avisar
// a ninguna otra placa.
// ============================================================================

// ============================== LED STRIP =====================================
const uint16_t PIXEL_COUNT = 100;
const uint8_t PIXEL_PIN = 5;

NeoPixelBus<NeoGrbFeature, NeoEsp32Rmt0800KbpsMethod> strip(PIXEL_COUNT, PIXEL_PIN);

RgbColor rojo(255, 0, 0);
RgbColor naranja(255, 80, 0);
RgbColor amarillo(255, 180, 0);
RgbColor apagado(0, 0, 0);

// ============================== TOUCH SENSOR ==================================
// Sensor tactil externo tipo TTP223
// VCC -> 3V3
// GND -> GND
// OUT -> D4 / GPIO4
const uint8_t TOUCH_PIN = 4;
const uint32_t TOUCH_DEBOUNCE_MS = 1200;

bool systemReady = false;
uint32_t lastTouchMs = 0;

// ============================== TIMING =========================================
const uint32_t ANIMATION_TOTAL_MS = 15000; // duracion de cada animacion "erase"

volatile bool pendingStart = false;

// ============================== LED FUNCTIONS =================================

void fillColor(const RgbColor& color) {
  for (uint16_t i = 0; i < PIXEL_COUNT; i++) {
    strip.SetPixelColor(i, color);
  }
  strip.Show();
}

void setMirrorStep(uint16_t i, const RgbColor& color) {
  uint16_t leftIndex = i;
  uint16_t rightIndex = (PIXEL_COUNT - 1) - i;

  strip.SetPixelColor(leftIndex, color);
  strip.SetPixelColor(rightIndex, color);
}

void eraseInHaciaCentro(const RgbColor& color, uint32_t totalMs) {
  const uint16_t steps = PIXEL_COUNT / 2;
  uint32_t stepDelay = totalMs / steps;

  if (stepDelay < 10) stepDelay = 10;

  for (uint16_t i = 0; i < steps; i++) {
    setMirrorStep(i, color);
    strip.Show();
    delay(stepDelay);
  }
}

void runSequence(uint32_t animationMs) {
  eraseInHaciaCentro(naranja, animationMs);
  eraseInHaciaCentro(amarillo, animationMs);
  eraseInHaciaCentro(rojo, animationMs);

  delay(1000);
  fillColor(apagado);
}

// ============================== TRIGGER ========================================

void triggerShow() {
  Serial.println("Sensor tocado: iniciando secuencia (modo individual)");
  pendingStart = true;
}

// ============================== TOUCH =========================================

bool touchDetected() {
  return digitalRead(TOUCH_PIN) == HIGH;
}

// ============================== SETUP / LOOP ==================================

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(TOUCH_PIN, INPUT);

  strip.Begin();
  fillColor(apagado);

  Serial.println("Placa 2 - Centro (Individual) lista");
  Serial.println("Esperando estabilizacion del sensor...");

  delay(1500);
  systemReady = true;

  Serial.println("Sistema listo. Toca el sensor para iniciar.");
}

void loop() {
  if (systemReady && touchDetected()) {
    uint32_t now = millis();

    if (now - lastTouchMs > TOUCH_DEBOUNCE_MS) {
      lastTouchMs = now;
      triggerShow();
    }
  }

  if (pendingStart) {
    noInterrupts();
    pendingStart = false;
    interrupts();

    fillColor(rojo);
    runSequence(ANIMATION_TOTAL_MS);
  }

  delay(10);
}
