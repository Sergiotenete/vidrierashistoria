/*
 * config.h — Instalación de una sola obra (vidriera) con 100 LED WS2812B
 *
 * Todos los ajustes de la instalación están en este fichero.
 * No hace falta tocar main.cpp para adaptar la obra.
 */

#pragma once

// ---------------------------------------------------------------------------
// 1. TIRA DE LED
// ---------------------------------------------------------------------------
#define NUM_LEDS              100      // número de WS2812B de la obra
#define PIN_DATOS             13       // GPIO de datos (evita 0, 2, 5, 12, 15: son strapping pins)
#define TIPO_LED              WS2812B
#define ORDEN_COLOR           GRB      // WS2812B = GRB. Si ves rojo/verde cambiados, prueba RGB

// ---------------------------------------------------------------------------
// 2. CONMUTADOR ON/OFF
// ---------------------------------------------------------------------------
#define PIN_CONMUTADOR        27       // GPIO al que va el conmutador (el otro polo a GND)

// 0 = conmutador de dos posiciones (interruptor/basculante que queda enclavado)
// 1 = pulsador momentáneo (cada pulsación alterna encendido/apagado)
#define MODO_CONMUTADOR       0

// 0 = contacto cerrado (GPIO a GND) significa ENCENDIDO
// 1 = invierte la lógica, por si el conmutador queda al revés en la caja
#define CONMUTADOR_INVERTIDO  0

#define ANTIRREBOTE_MS        60       // ms de señal estable antes de aceptar un cambio

// ---------------------------------------------------------------------------
// 3. POTENCIA (fuente de 5 V / 10 A)
// ---------------------------------------------------------------------------
#define VOLTIOS               5
// Techo de consumo que FastLED nunca sobrepasa. 6000 mA deja la fuente
// trabajando al ~60 % de su capacidad: menos calor, mucha más vida útil.
#define MAX_MILIAMPERIOS      6000
// Brillo máximo de la obra (0-255). 180 es luminoso de sobra para una vidriera
// y alarga la vida de la tira. Súbelo solo si el espacio está muy iluminado.
#define BRILLO_MAXIMO         180

// ---------------------------------------------------------------------------
// 4. FUNDIDOS
// ---------------------------------------------------------------------------
#define FADE_IN_MS            3000     // duración del fundido de entrada (encendido)
#define FADE_OUT_MS           4000     // duración del fundido de salida (apagado)
// 1 = corrige la curva para que el fundido se perciba lineal al ojo (recomendado)
#define APLICAR_GAMMA_FADE    1

// ---------------------------------------------------------------------------
// 5. EFECTO ARCOÍRIS
// ---------------------------------------------------------------------------
#define FPS                   100      // refrescos por segundo
// Avance de tono por frame en 1/256 de unidad. 40 a 100 FPS => ciclo completo
// de color cada ~16 s. Baja el valor para un movimiento más contemplativo.
#define VELOCIDAD_ARCOIRIS    40
// Salto de tono entre LED contiguos. Con 100 LED: 1 = degradado muy suave,
// 3 = un arcoíris completo repartido a lo largo de la obra.
#define EXTENSION_ARCOIRIS    2
#define SATURACION            255      // 255 = colores plenos; 200-230 = más pastel

// ---------------------------------------------------------------------------
// 6. CORTE FÍSICO DE ALIMENTACIÓN DE LA TIRA (opcional)
// ---------------------------------------------------------------------------
// Si lo activas, un relé o MOSFET de LADO ALTO (sobre el +5 V, nunca sobre GND)
// corta la alimentación de la tira al terminar el fundido de salida.
// Elimina los ~100 mA de reposo de los 100 WS2812B y evita que envejezcan
// mientras la obra está apagada. Ver docs/02-esp32-24-7.md.
#define USAR_CORTE_ALIMENTACION 0
#define PIN_ALIM_LEDS         26
#define ALIM_ACTIVA_EN        HIGH     // nivel del GPIO que ACTIVA el relé/MOSFET
#define RETARDO_ALIM_MS       40       // espera tras dar tensión antes del primer dato

// ---------------------------------------------------------------------------
// 7. FUNCIONAMIENTO 24/7
// ---------------------------------------------------------------------------
#define ESPERA_ARRANQUE_MS    500      // deja estabilizar la fuente antes del primer envío
#define WDT_SEGUNDOS          8        // watchdog: reinicia solo si el firmware se cuelga
// 0 = no tocar la frecuencia de CPU (240 MHz, por defecto).
// 80 = menos consumo y menos calor. Si aparece parpadeo, vuelve a 0.
#define FRECUENCIA_CPU_MHZ    0
#define DESACTIVAR_RADIO      1        // apaga WiFi y Bluetooth (no se usan)

// ---------------------------------------------------------------------------
// 8. REPOSO DEL ESP32 (standby hasta recibir la señal del conmutador)
// ---------------------------------------------------------------------------
// 0 = SIN REPOSO. El ESP32 sigue ejecutando con la obra apagada (por defecto).
//     Respuesta instantánea, consola serie siempre disponible.
//
// 1 = LIGHT SLEEP. Se duerme al terminar el fundido de salida y despierta por
//     nivel en PIN_CONMUTADOR REANUDANDO la ejecución donde la dejó. El chip
//     baja a ~1 mA y el despertar es inmediato.
//     ATENCIÓN: hay que verificarlo en tu placa. El ESP32 puede perder la
//     configuración del periférico RMT que FastLED usa para la tira, y FastLED
//     no ofrece una forma limpia de reinicializarlo. Si tras el primer ciclo
//     de reposo la tira no responde o muestra basura, usa el modo 2.
//
// 2 = DEEP SLEEP. El chip baja a ~10 µA, pero al despertar la placa REARRANCA
//     desde setup(). Es la opción fiable: el rearranque deja todos los
//     periféricos limpios. Coste: ~0,3-0,5 s desde que accionas el conmutador
//     hasta que empieza el fundido de entrada.
//     Requiere que PIN_CONMUTADOR sea un GPIO del dominio RTC
//     (0, 2, 4, 12-15, 25-27, 32-39). El 27 por defecto lo es.
#define MODO_REPOSO           0

// Tiempo que la obra debe llevar apagada antes de que el ESP32 se duerma.
// Además de dar margen para grabar firmware, hace que un despertar espurio
// (ruido en un cable largo hasta el conmutador) vuelva solo al reposo.
#define RETARDO_REPOSO_MS     2000

// Con MODO_REPOSO 2 puedes bajar ESPERA_ARRANQUE_MS a ~150: al despertar la
// obra arranca siempre desde negro, así que no hay pico de corriente que
// amortiguar y se recorta el retardo perceptible.
