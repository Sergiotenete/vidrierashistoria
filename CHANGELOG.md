# Changelog

Todos los cambios relevantes de este proyecto se documentan en este archivo.

## [Unreleased]
### Added
- Separación del código en tres sketches independientes:
  - Placa 1 - Izquierda, Receptor.
  - Placa 2 - Centro, Emisor.
  - Placa 3 - Derecha, Receptor.
- Archivo `INDICE.md` con estructura, flujo y parámetros clave.
- Referencias en `README.md` al índice y al changelog.
- Nuevo sketch `sketches/placa2_centro_individual/placa2_centro_individual.ino`:
  versión autónoma de la Placa 2 (Centro), sin WiFi ni ESP-NOW. Basado en
  `centro-v2-funciona`, con sensor táctil externo TTP223 (GPIO4). Al tocar
  el sensor ejecuta su propia animación de inmediato, sin coordinar con
  las placas Izquierda/Derecha. Pensado para pruebas o uso del cuadro
  central de forma independiente.

### Changed
- Se reemplaza el enfoque de sketch único con roles por archivos independientes por placa.
- Se mantiene la lógica de arranque escalonado (0s, +3s, +6s) y animación espejo de 100 LEDs.
