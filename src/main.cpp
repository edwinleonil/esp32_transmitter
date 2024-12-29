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

// Add these defines at the top with other constants
#define FORWARD_BTN 12  // Forward button pin
#define BACKWARD_BTN 13 // Backward button pin
#define DEBOUNCE_TIME 50 // Debounce time in milliseconds

// Add these variables for button debouncing
unsigned long lastDebounceTime = 0;
bool lastForwardState = HIGH;
bool lastBackwardState = HIGH;

typedef struct struct_message {
  int16_t speedVal;  // -255..255
} struct_message;

struct_message myData = {0};

// Update with your Receiver's MAC address
uint8_t peerAddress[] = {0x08, 0xD1, 0xF9, 0xEC, 0xFB, 0x34};

const int SPEED_STEP = 100;  // how much to change speed per press

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
  // Add button pin setup
  pinMode(FORWARD_BTN, INPUT_PULLUP);
  pinMode(BACKWARD_BTN, INPUT_PULLUP);
  
  Serial.println("Transmitter ready. Use buttons to control speed.");
}

void loop() {
  // Read button states (LOW when pressed because of pull-up)
  bool forwardState = digitalRead(FORWARD_BTN);
  bool backwardState = digitalRead(BACKWARD_BTN);
  
  // Check if enough time has passed since last button press
  if ((millis() - lastDebounceTime) > DEBOUNCE_TIME) {
    // Forward button pressed
    if (forwardState == LOW && lastForwardState == HIGH) {
      // if we're negative, reset to 0 before going forward
      if (myData.speedVal < 0) myData.speedVal = 0;
      myData.speedVal += SPEED_STEP;
      if (myData.speedVal > 255) myData.speedVal = 255; 
      Serial.print("Forward speed -> ");
      Serial.println(myData.speedVal);
      lastDebounceTime = millis();
    }
    // Backward button pressed
    else if (backwardState == LOW && lastBackwardState == HIGH) {
      // if we're positive, reset to 0 before going backward
      if (myData.speedVal > 0) myData.speedVal = 0;
      myData.speedVal -= SPEED_STEP;
      if (myData.speedVal < -255) myData.speedVal = -255;
      Serial.print("Backward speed -> ");
      Serial.println(myData.speedVal);
      lastDebounceTime = millis();
    }
    // Both buttons pressed - stop
    if (forwardState == LOW && backwardState == LOW) {
      myData.speedVal = 0;
      Serial.println("Stopped (speed=0)");
      lastDebounceTime = millis();
    }

    // If speed was changed, send the update
    if (forwardState != lastForwardState || backwardState != lastBackwardState) {
      esp_err_t result = esp_now_send(peerAddress, (uint8_t *)&myData, sizeof(myData));
      if (result == ESP_OK) {
        Serial.println("Data sent successfully.");
      } else {
        Serial.println("Error sending data.");
      }
    }
  }

  // Save button states for next iteration
  lastForwardState = forwardState;
  lastBackwardState = backwardState;
}