/*
 *  ESP32 Receiver using ESP-NOW
 *  - Sets an LED connected to GPIO 18 to ON or OFF based on received data
 */

#include <esp_now.h>
#include <WiFi.h>

// Structure matching the transmitter
typedef struct struct_message {
  bool ledState;  // true = ON, false = OFF
} struct_message;

// Create a struct_message to hold the incoming data
struct_message incomingData;

// Callback function that will be executed when data is received
void onDataRecv(const uint8_t * mac, const uint8_t *incomingDataBytes, int len) {
  // Copy incoming bytes into our structure
  memcpy(&incomingData, incomingDataBytes, sizeof(incomingData));

  Serial.print("Data received from MAC: ");
  Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X\n", 
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  // Set LED state
  if (incomingData.ledState) {
    digitalWrite(18, HIGH);
    Serial.println("LED turned ON");
  } else {
    digitalWrite(18, LOW);
    Serial.println("LED turned OFF");
  }
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initialize LED pin
  pinMode(18, OUTPUT);
  digitalWrite(18, LOW); // default OFF

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the receive callback function
  esp_now_register_recv_cb(onDataRecv);

  Serial.println("Receiver ready. LED on GPIO 18 is set OFF by default.");
}

void loop() {
  // In this example, nothing to do here:
  // The onDataRecv callback handles everything.
  delay(1000);
}
