/*
 *  ESP32 Transmitter using ESP-NOW
 *  - Type 'u' in Serial Monitor to send LED ON
 *  - Type 'd' in Serial Monitor to send LED OFF
 */

#include <esp_now.h>
#include <WiFi.h>

// Define a data structure to send
typedef struct struct_message {
  bool ledState;  // true = ON, false = OFF
} struct_message;

// Create a struct_message to hold the data we're sending
struct_message myData = {false};

// MAC address of the receiver (ESP32 #2). Updated to: 08:D1:F9:EC:FB:34
uint8_t peerAddress[] = {0x08, 0xD1, 0xF9, 0xEC, 0xFB, 0x34}; 

// Callback when data is sent
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status: ");
  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("Delivery Success");
  } else {
    Serial.println("Delivery Fail");
  }
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

  // Register callback to get the status of transmitted packets
  esp_now_register_send_cb(onDataSent);

  // Register peer
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, peerAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("Transmitter ready. Type 'u' (ON) or 'd' (OFF):");
}

void loop() {
  // Check if there’s any data available from the Serial Monitor
  if (Serial.available() > 0) {
    char c = (char)Serial.read();
    
    // Turn LED ON if user typed 'u'
    if (c == 'u' || c == 'U') {
      myData.ledState = true;
      Serial.println("Sending ON signal...");
    }
    // Turn LED OFF if user typed 'd'
    else if (c == 'd' || c == 'D') {
      myData.ledState = false;
      Serial.println("Sending OFF signal...");
    }
    // Ignore other characters
    else {
      Serial.println("Type 'u' or 'd' to change LED state.");
      return;
    }

    // Send the updated data to the receiver
    esp_err_t result = esp_now_send(peerAddress, (uint8_t *)&myData, sizeof(myData));
    if (result == ESP_OK) {
      Serial.println("Data sent successfully.");
    } else {
      Serial.println("Error sending the data.");
    }
  }
}
