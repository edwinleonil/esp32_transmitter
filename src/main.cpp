/*
 *  TRANSMITTER (ESP32 #1)
 *  Sends speedVal in [-255..255]:
 *    Positive => forward
 *    Negative => backward
 *    Zero     => stop
 *  
 *  Type in Serial Monitor:
 *    'f' to increase forward speed
 *    'b' to increase backward speed
 *    's' to stop
 */

#include <esp_now.h>
#include <WiFi.h>

typedef struct struct_message {
  int16_t speedVal;  // -255..255
} struct_message;

struct_message myData = {0};

// Update with your Receiver's MAC address
uint8_t peerAddress[] = {0x08, 0xD1, 0xF9, 0xEC, 0xFB, 0x34};

const int SPEED_STEP = 25;  // how much to change speed per press

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(onDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, peerAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("Transmitter ready. Commands: 'f' forward, 'b' backward, 's' stop.");
}

void loop() {
  if (Serial.available() > 0) {
    char c = (char)Serial.read();

    if (c == 'f' || c == 'F') {
      // if we're negative, reset to 0 before going forward
      if (myData.speedVal < 0) myData.speedVal = 0;
      myData.speedVal += SPEED_STEP;
      if (myData.speedVal > 255) myData.speedVal = 255; 
      Serial.print("Forward speed -> ");
      Serial.println(myData.speedVal);
    }
    else if (c == 'b' || c == 'B') {
      // if we're positive, reset to 0 before going backward
      if (myData.speedVal > 0) myData.speedVal = 0;
      myData.speedVal -= SPEED_STEP;
      if (myData.speedVal < -255) myData.speedVal = -255;
      Serial.print("Backward speed -> ");
      Serial.println(myData.speedVal);
    }
    else if (c == 's' || c == 'S') {
      myData.speedVal = 0;
      Serial.println("Stopped (speed=0)");
    }
    else {
      Serial.println("Invalid command. Use 'f', 'b', or 's'.");
      return;
    }

    // Send updated speed to the receiver
    esp_err_t result = esp_now_send(peerAddress, (uint8_t *)&myData, sizeof(myData));
    if (result == ESP_OK) {
      Serial.println("Data sent successfully.");
    } else {
      Serial.println("Error sending data.");
    }
  }
}
