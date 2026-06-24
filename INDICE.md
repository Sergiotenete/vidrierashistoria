# Índice del proyecto

## 1) Objetivo
Sistema de tres cuadros con ESP32 + WS2812B, sincronizados por ESP-NOW y disparados por touch en el cuadro central.

## 2) Distribución por placas
- **Placa 1 (Izquierda / Receptor):** `sketches/placa1_izquierda_receptor/placa1_izquierda_receptor.ino`
- **Placa 2 (Centro / Emisor):** `sketches/placa2_centro_emisor/placa2_centro_emisor.ino`
- **Placa 3 (Derecha / Receptor):** `sketches/placa3_derecha_receptor/placa3_derecha_receptor.ino`

## 3) Flujo de ejecución
1. Touch en placa central.
2. Central envía comando ESP-NOW a izquierda con `delay=0`.
3. Central envía comando ESP-NOW a derecha con `delay=3000`.
4. Central arranca localmente con `delay=6000`.

## 4) Parámetros clave
- `PIXEL_COUNT = 100`
- `STAGGER_MS = 3000`
- `ANIMATION_TOTAL_MS = 15000`
- `TOUCH_THRESHOLD` (calibrable)

## 5) Control de versiones
Ver `CHANGELOG.md` para histórico de cambios.
