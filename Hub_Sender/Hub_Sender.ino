#include <esp_now.h> //Needed for ESPNow
#include <WiFi.h> //Needed for ESPNow
#include <Arduino.h>
#include <Firebase_ESP_Client.h>


// Connecting to firebase
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "gabeDevice"
#define WIFI_PASSWORD "password"

// Insert Firaebase project API Key
#define API_KEY "AIzaSyAG2ogN6DiJ8EJbh5sTSwHCPPPIWXod3ZE"
// Insert RTDB URLefine the RTDB URL/
#define DATABASE_URL "https://library-seating-system-default-rtdb.firebaseio.com/" 

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

bool espNowMode = true;


// This is talking between two ESP32's
uint8_t receiverMac[] =  {0xA0, 0xB7, 0x65, 0xFE, 0x7B, 0x88}; // Replace with the MAC address of the receiving ESP32 uint8_t senderMac[] = {0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56};   // Replace with the MAC address of the sending ESP32
 // uint8_t senderMac[] = {0xA0, 0xB7, 0x65, 0xFE, 0xD6, 0xFC};
// Define variables to store incoming message
//String incomingMessage;

esp_now_peer_info_t peerInfo;

typedef struct struct_message {
  uint8_t command;
  char data[32];
} struct_message;


struct_message incomingMessage;
// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  //struct_message incomingMessage;
  memcpy(&incomingMessage, incomingData, sizeof(incomingMessage));
  // Parse and process the received data
  uint8_t receivedCommand = incomingMessage.command;
  String receivedData = String(incomingMessage.data);

  Serial.print("Received Command: ");
  Serial.println(receivedCommand);
  Serial.print("Received Data: ");
  Serial.println(receivedData);
  // Perform actions based on receivedCommand and receivedData here
}

void setup() {
 Serial.begin(115200);

if (espNowMode) {
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
   // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, receiverMac, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
