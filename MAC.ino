#include <esp_now.h>
#include <WiFi.h>

#define BUTTON_PIN 4
#define LED_PIN 5

uint8_t receiverMAC[] = {0xFC, 0xB4, 0x67, 0x5A, 0x8F, 0xE0};  // Replace with other ESP32's MAC 4c:eb:d6:78:cd:a8(sir2)
bool ledState = false;
unsigned long lastPressTime = 0;
const int debounceDelay = 500;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Send Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void OnDataRecv(const esp_now_recv_info_t *recvInfo, const uint8_t *incomingData, int len) {
    Serial.println("Data Received");
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW Init Failed");
        return;
    }

    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, receiverMAC, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }
}

void loop() {
    if (digitalRead(BUTTON_PIN) == LOW && millis() - lastPressTime > debounceDelay) {
        lastPressTime = millis();
        Serial.println("Button Pressed - Sending Signal");
        esp_now_send(receiverMAC, (uint8_t *)"toggle", sizeof("toggle"));
    }
}

