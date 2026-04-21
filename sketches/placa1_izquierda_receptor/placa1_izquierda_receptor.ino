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

// ============================ RED / ESP-NOW ===================================
struct __attribute__((packed)) CommandMessage {
  uint8_t command;
  uint32_t delayMs;
  uint32_t animationMs;
};

const uint8_t CMD_START_ANIMATION = 1;

volatile bool pendingStart = false;
volatile uint32_t pendingDelayMs = 0;
volatile uint32_t pendingAnimationMs = 15000;

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

void onDataRecv(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
  if (len != sizeof(CommandMessage)) return;

  CommandMessage msg;
  memcpy(&msg, data, sizeof(msg));

  if (msg.command == CMD_START_ANIMATION) {
    pendingDelayMs = msg.delayMs;
    pendingAnimationMs = msg.animationMs;
    pendingStart = true;
  }
}

void setupEspNow() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error inicializando ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(onDataRecv);
}

void printMac() {
  uint8_t mac[6];
  WiFi.macAddress(mac);
  Serial.printf("MAC Placa 1 (Izquierda): %02X:%02X:%02X:%02X:%02X:%02X\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  strip.Begin();
  fillColor(apagado);

  setupEspNow();
  printMac();
  Serial.println("Placa 1 - Izquierda (Receptor) lista");
}

void loop() {
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
