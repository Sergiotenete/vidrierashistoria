#include <WiFi.h>
#include <esp_now.h>
#include <NeoPixelBus.h>

// ============================== LED STRIP =====================================
const uint16_t PIXEL_COUNT = 100;
const uint8_t PIXEL_PIN = 5;
NeoPixelBus<NeoGrbFeature, NeoEsp32Rmt0800KbpsMethod> strip(PIXEL_COUNT, PIXEL_PIN);

RgbColor rojo(255, 0, 0);
RgbColor naranja(255, 80, 0);
RgbColor amarillo(255, 180, 0);
RgbColor apagado(0, 0, 0);

// ============================== TOUCH SENSOR ==================================
const uint8_t TOUCH_PIN = T0;          // GPIO4
const uint16_t TOUCH_THRESHOLD = 28;   // calibrar en placa real
const uint32_t TOUCH_DEBOUNCE_MS = 1200;

// ============================ RED / ESP-NOW ===================================
// Reemplaza por MAC reales de Placa 1 y Placa 3.
uint8_t MAC_IZQUIERDA[6] = {0x48, 0x9D, 0x31, 0xC0, 0x50, 0x20};
uint8_t MAC_DERECHA[6]   = {0x1C, 0xC3, 0xAB, 0xD2, 0x92, 0x1C}; 

struct __attribute__((packed)) CommandMessage {
  uint8_t command;
  uint32_t delayMs;
  uint32_t animationMs;
};

const uint8_t CMD_START_ANIMATION = 1;
const uint32_t STAGGER_MS = 3000;
const uint32_t ANIMATION_TOTAL_MS = 15000;

volatile bool pendingStart = false;
volatile uint32_t pendingDelayMs = 0;
volatile uint32_t pendingAnimationMs = ANIMATION_TOTAL_MS;

uint32_t lastTouchMs = 0;

void fillColor(const RgbColor& color) {
  for (uint16_t i = 0; i < PIXEL_COUNT; i++) {
    strip.SetPixelColor(i, color);
  }
  strip.Show();
}

void setMirrorStep(uint16_t i, const RgbColor& color) {
  // Mapeo vertical en espejo: 0->49 y 99->50
  uint16_t leftIndex = i;
  uint16_t rightIndex = (PIXEL_COUNT - 1) - i;
  strip.SetPixelColor(leftIndex, color);
  strip.SetPixelColor(rightIndex, color);
}

void eraseInHaciaCentro(const RgbColor& color, uint32_t totalMs) {
  const uint16_t steps = PIXEL_COUNT / 2; // 50 pasos
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
}

bool addPeer(const uint8_t* macAddress) {
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, macAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  esp_err_t result = esp_now_add_peer(&peerInfo);
  return result == ESP_OK || result == ESP_ERR_ESPNOW_EXIST;
}

void sendStartTo(const uint8_t* targetMac, uint32_t delayMs, uint32_t animationMs) {
  CommandMessage msg;
  msg.command = CMD_START_ANIMATION;
  msg.delayMs = delayMs;
  msg.animationMs = animationMs;

  esp_now_send(targetMac, reinterpret_cast<const uint8_t*>(&msg), sizeof(msg));
}

void triggerShow() {
  // Orden solicitado:
  // 1) Izquierda ahora
  // 2) Derecha +3s
  // 3) Centro +6s
  sendStartTo(MAC_IZQUIERDA, 0, ANIMATION_TOTAL_MS);
  sendStartTo(MAC_DERECHA, STAGGER_MS, ANIMATION_TOTAL_MS);

  pendingDelayMs = STAGGER_MS * 2;
  pendingAnimationMs = ANIMATION_TOTAL_MS;
  pendingStart = true;
}

void setupEspNow() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error inicializando ESP-NOW");
    return;
  }

  if (!addPeer(MAC_IZQUIERDA)) {
    Serial.println("No se pudo agregar peer IZQUIERDA");
  }
  if (!addPeer(MAC_DERECHA)) {
    Serial.println("No se pudo agregar peer DERECHA");
  }
}

bool touchDetected() {
  uint16_t value = touchRead(TOUCH_PIN);
  return value < TOUCH_THRESHOLD;
}

void printMac() {
  uint8_t mac[6];
  WiFi.macAddress(mac);
  Serial.printf("MAC Placa 2 (Centro): %02X:%02X:%02X:%02X:%02X:%02X\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  strip.Begin();
  fillColor(apagado);

  setupEspNow();
  printMac();

  Serial.println("Placa 2 - Centro (Emisor) lista");
  Serial.println("Toca el sensor para disparar la secuencia");
}

void loop() {
  if (touchDetected()) {
    uint32_t now = millis();
    if (now - lastTouchMs > TOUCH_DEBOUNCE_MS) {
      lastTouchMs = now;
      triggerShow();
    }
  }

  if (pendingStart) {
    noInterrupts();
    uint32_t delayLocal = pendingDelayMs;
    uint32_t animationMs = pendingAnimationMs;
    pendingStart = false;
    interrupts();

    if (delayLocal > 0) delay(delayLocal);

    fillColor(rojo);
    runSequence(animationMs);
  }

  delay(10);
}
