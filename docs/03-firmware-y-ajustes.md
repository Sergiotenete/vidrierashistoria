# 03 — Firmware, grabación y ajustes

## Cómo funciona

Máquina de estados de cuatro posiciones que se refresca 100 veces por segundo:

```
        conmutador ON                    fundido completado
 APAGADO ──────────────► ENCENDIENDO ────────────────────► ENCENDIDO
    ▲                         │                                 │
    │                         │ conmutador OFF                  │ conmutador OFF
    │  fundido completado     ▼                                 │
    └──────────────────── APAGANDO ◄──────────────────────────────
```

- **Efecto:** arcoíris continuo. Cada LED recibe un tono desplazado respecto al
  anterior (`EXTENSION_ARCOIRIS`) y el conjunto avanza en el círculo cromático
  (`VELOCIDAD_ARCOIRIS`), de modo que el color recorre la obra sin saltos ni repeticiones.
- **Fundidos:** se aplican como una envolvente global de brillo sobre el efecto, con una
  curva `ease-in-out` cúbica y corrección gamma. El resultado es un fundido que el ojo
  percibe uniforme, en lugar de arrancar de golpe y arrastrarse al final.
- **Interrupciones:** si accionas el conmutador a mitad de un fundido, la transición
  arranca desde el brillo real en ese instante y se escala el tiempo restante. Nunca hay
  saltos de luz.
- **Tras un corte de luz:** si el conmutador estaba en ON, la obra entra con su fundido
  normal, no de golpe. Esto también evita el pico de corriente en la fuente.

---

## Grabar el firmware

### Con PlatformIO (recomendado)

```bash
cd firmware/obra_unica
pio run -t upload      # compila y graba
pio device monitor     # consola serie a 115200 baudios
```

La consola informa de cada transición:

```
[obra] Vidrieras Historia — obra única, 100 x WS2812B
[obra] lista
[obra] fundido de entrada
[obra] encendida
[obra] fundido de salida
[obra] apagada
```

### Con el IDE de Arduino

1. Instala el soporte para ESP32 (*Gestor de tarjetas* → `esp32` de Espressif).
2. Instala la biblioteca **FastLED** (3.6 o superior) desde el gestor de bibliotecas.
3. Crea una carpeta `obra_unica` y copia dentro:
   - `src/main.cpp` renombrado a **`obra_unica.ino`**
   - `include/config.h` como **`config.h`**
4. Placa: *ESP32 Dev Module*. Velocidad de subida: 921600.

> Durante la grabación, desconecta el +5 V de la fuente del pin `5V` de la placa y
> alimenta solo por USB. No conviene tener las dos fuentes en paralelo.

---

## Ajustes en `config.h`

Todo lo que se toca en obra está en un único fichero.

### Los cuatro parámetros que vas a querer cambiar

| Parámetro | Por defecto | Qué hace |
|---|---|---|
| `BRILLO_MAXIMO` | `150` | Brillo de la obra a plena luz (0-255). Ajuste estético principal y el que más influye en la vida de los LED (doc 05) |
| `FADE_IN_MS` | `3000` | Duración del fundido de entrada, en milisegundos |
| `FADE_OUT_MS` | `4000` | Duración del fundido de salida |
| `VELOCIDAD_ARCOIRIS` | `40` | Velocidad del color. 40 → ciclo completo cada ~16 s. Bájalo a 15-20 para un ritmo más contemplativo |

### Resto de ajustes

| Parámetro | Por defecto | Notas |
|---|---|---|
| `NUM_LEDS` | `100` | Si cambias la tira, cambia también el presupuesto de potencia |
| `PIN_DATOS` | `13` | Evita GPIO 0, 2, 5, 12 y 15: son *strapping pins* |
| `PIN_CONMUTADOR` | `27` | Con *pull-up* interno; el otro polo del conmutador a GND |
| `MODO_CONMUTADOR` | `0` | `0` interruptor enclavado · `1` pulsador momentáneo |
| `CONMUTADOR_INVERTIDO` | `0` | Ponlo a `1` si la lógica queda al revés al montarlo |
| `ANTIRREBOTE_MS` | `60` | Súbelo si el conmutador es viejo o el cable es largo |
| `MAX_MILIAMPERIOS` | `6000` | Techo de consumo. No pasar de 8000 con la fuente de 10 A |
| `EXTENSION_ARCOIRIS` | `2` | `1` degradado muy suave · `3` un arcoíris completo en la obra |
| `SATURACION` | `255` | Bájalo a 200-230 si quieres una luz más pastel que deje leer mejor el vidrio |
| `APLICAR_GAMMA_FADE` | `1` | Corrección perceptual del fundido. Déjalo a `1` |
| `USAR_CORTE_ALIMENTACION` | `0` | Relé/MOSFET de lado alto sobre el +5 V de la tira (ver doc 02, opción C) |
| `MODO_REPOSO` | `0` | `0` sin reposo · `1` light sleep · `2` deep sleep (ver doc 04) |
| `RETARDO_REPOSO_MS` | `2000` | Tiempo con la obra apagada antes de dormirse |
| `APAGADO_AUTOMATICO_H` | `0` | Horas de encendido continuo tras las que la obra se apaga sola. `0` = desactivado. La medida más eficaz para alargar la vida de los LED (doc 05) |
| `FRECUENCIA_CPU_MHZ` | `0` | `0` = 240 MHz. Ponlo a `80` para menos calor; vuelve a `0` si aparece parpadeo |
| `DESACTIVAR_RADIO` | `1` | Apaga WiFi y Bluetooth |
| `WDT_SEGUNDOS` | `8` | Watchdog: reinicia la placa si el firmware se cuelga |

---

## Averías habituales

| Síntoma | Causa probable | Solución |
|---|---|---|
| La tira no enciende, ni un LED | Datos por el extremo equivocado (DO en vez de DIN) | Mira las flechas impresas en la tira |
| Solo enciende el primer LED | Falta masa común entre ESP32 y fuente | Une los GND |
| Parpadeos aleatorios | Nivel de datos insuficiente a 3,3 V | Monta el 74AHCT125 (doc 01, §4) |
| Los colores están cambiados | Orden de color equivocado | Prueba `ORDEN_COLOR RGB` en `config.h` |
| El final de la tira tira a rojo y apagado | Caída de tensión | Inyecta corriente en el extremo final |
| La placa se reinicia al encender la obra | Caída del raíl de 5 V por el pico de corriente | Condensadores de desacoplo; o fuente separada (doc 02, opción B) |
| El conmutador actúa al revés | Cableado invertido | `CONMUTADOR_INVERTIDO 1` |
| El conmutador cambia solo | Ruido en un cable largo | Pull-up externo de 10 kΩ + 100 nF, cable apantallado |
| El fundido de salida no llega a verse | El conmutador está cortando la alimentación | Es la topología B; recablea como topología A (doc 01, §5) |
| Con `MODO_REPOSO 1`, la tira no responde tras el primer reposo | El light sleep ha dejado el periférico RMT en estado indefinido | Usa `MODO_REPOSO 2` (doc 04) |
| Con `MODO_REPOSO 2`, la obra se enciende sola de vez en cuando | Ruido en el cable del conmutador despertando al ESP32 | Pull-up externo de 10 kΩ + 100 nF (doc 01, §5) |
| El firmware no compila: «debe ser un GPIO del dominio RTC» | Deep sleep con un pin de conmutador sin dominio RTC | Usa 0, 2, 4, 12-15, 25-27 o 32-39 |
| La obra se apagó sola y no responde al conmutador | `APAGADO_AUTOMATICO_H` ha llegado a su límite | Es lo esperado: pasa el conmutador por OFF y vuelve a ON |
