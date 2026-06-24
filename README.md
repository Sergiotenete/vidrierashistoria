# vidrierashistoria

Proyecto de 3 ESP32-WROOM con tiras WS2812B (100 LEDs por cuadro) y disparo por sensor táctil en la placa central.

## Estructura solicitada (código independiente por placa)

- **Placa 1 - Izquierda, Receptor**  
  `sketches/placa1_izquierda_receptor/placa1_izquierda_receptor.ino`
- **Placa 2 - Centro, Emisor**  
  `sketches/placa2_centro_emisor/placa2_centro_emisor.ino`
- **Placa 3 - Derecha, Receptor**  
  `sketches/placa3_derecha_receptor/placa3_derecha_receptor.ino`

## Versión individual (sin ESP-NOW)

- **Placa 2 - Centro, Individual / Autónoma**  
  `sketches/placa2_centro_individual/placa2_centro_individual.ino`

Versión standalone de la placa central para usarse sola, sin las placas
Izquierda/Derecha. No usa WiFi ni ESP-NOW: al tocar el sensor táctil
ejecuta su propia animación de inmediato (sin los retardos de 3s/6s,
ya que no hay nada que coordinar con otras placas). Útil para pruebas
de banco o para montajes donde solo se necesita el cuadro central.

## Comportamiento

Al tocar el sensor en la placa central:
1. Inicia la izquierda (inmediato).
2. A los 3 segundos inicia la derecha.
3. A los 3 segundos adicionales inicia el centro.

Las animaciones se ejecutan con efecto espejo vertical por pares:
- `0 -> 49`
- `99 -> 50`

## Documentación

- Índice general: `INDICE.md`
- Historial de cambios y versiones: `CHANGELOG.md`

## Librerías necesarias

- `NeoPixelBus`
- `WiFi` (ESP32 core)
- `esp_now` (ESP32 core)

## Configuración rápida

1. Cargar cada sketch en su placa correspondiente.
2. Encender y abrir monitor serie para obtener MAC de cada receptor.
3. Editar en la placa central:
   - `MAC_IZQUIERDA`
   - `MAC_DERECHA`
4. Ajustar si hace falta:
   - `TOUCH_THRESHOLD`
   - `STAGGER_MS` (3s)
   - `ANIMATION_TOTAL_MS` (15s por animación `erase`)
