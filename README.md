# vidrierashistoria

Instalaciones lumínicas para vidrieras históricas.

## Obra única — 100 LED WS2812B con conmutador on/off

Instalación de una sola obra iluminada por detrás con una tira de 100 LED WS2812B
gobernada por un ESP32. Un conmutador on/off enciende y apaga la pieza con **fundido de
entrada y fundido de salida**, mostrando un **arcoíris en movimiento continuo** mientras
está encendida.

- Fuente: **5 V / 10 A**, trabajando por debajo del 60 % de su capacidad.
- El ESP32 permanece **encendido de forma permanente**; el conmutador es una señal, no
  un corte de corriente — por eso el fundido de salida puede completarse.
- Consumo en reposo con la obra apagada: **~0,9 W**.

### Estructura

```
firmware/obra_unica/
  platformio.ini          proyecto PlatformIO (ESP32 + FastLED)
  include/config.h        todos los ajustes de la instalación
  src/main.cpp            firmware
docs/
  01-instalacion-electrica.md   materiales, esquema, cableado y montaje
  02-esp32-24-7.md              opciones para tenerlo siempre encendido sin que sufra
  03-firmware-y-ajustes.md      grabación, parámetros y averías habituales
```

### Arranque rápido

```bash
cd firmware/obra_unica
pio run -t upload
pio device monitor
```

Antes de dar tensión por primera vez, sigue la lista de comprobación de
[docs/01-instalacion-electrica.md](docs/01-instalacion-electrica.md#8-puesta-en-marcha-lista-de-comprobación).
