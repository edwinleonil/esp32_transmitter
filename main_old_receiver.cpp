/*
 *  ESP32 Receiver using ESP-NOW
 */

#include <esp_now.h>
#include <WiFi.h>

// - Blue receiver: 08:D1:F9:EC:FB:34
// - Red transmitter:  B0:A7:32:2E:44:8C

// Structure matching the one from the transmitter
typedef struct struct_message {
  char msg[32];
} struct_message;

// Create a struct_message to hold the incoming data
struct_message incomingData;

// Callback function that will be executed when data is received
void onDataRecv(const uint8_t * mac, const uint8_t *incomingDataBytes, int len) {
  // Copy incoming bytes into our structure
  memcpy(&incomingData, incomingDataBytes, sizeof(incomingData));

  Serial.print("Bytes received from: ");
  Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X \n", 
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  Serial.print("Message: ");
  Serial.println(incomingData.msg);
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the receive callback function
  esp_now_register_recv_cb(onDataRecv);
}

void loop() {
  // In this example, we just wait for incoming data. 
  // The onDataRecv callback handles everything else.
  delay(1000);
}
