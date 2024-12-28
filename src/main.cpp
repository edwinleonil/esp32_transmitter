/*
 *  ESP32 Transmitter using ESP-NOW
 */

#include <esp_now.h>
#include <WiFi.h>

// Structure to send data
typedef struct struct_message {
  char msg[32];
} struct_message;

// Create a struct_message to hold the message we're sending
struct_message myData;

// MAC address of the receiver (ESP32 #2). Replace with the actual address.
// - Blue receiver: 08:D1:F9:EC:FB:34
// - Red transmitter:  B0:A7:32:2E:44:8C
// Example: "24:6F:28:xx:xx:xx" => {0x24, 0x6F, 0x28, 0xxx, 0xxx, 0xxx}
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
  Serial.begin(9600);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of the transmitted packet
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
}

void loop() {
  // Prepare data to send
  strcpy(myData.msg, "Hello from ESP32 #1");

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(peerAddress, (uint8_t *) &myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }

  // Wait a bit before sending next message
  delay(2000);
}
