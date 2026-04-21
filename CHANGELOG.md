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

### Changed
- Se reemplaza el enfoque de sketch único con roles por archivos independientes por placa.
- Se mantiene la lógica de arranque escalonado (0s, +3s, +6s) y animación espejo de 100 LEDs.
