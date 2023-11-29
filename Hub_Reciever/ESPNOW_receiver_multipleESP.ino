#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH THE MAC Addresses of your receivers
uint8_t receiverMac1[] = {0xA0, 0xB7, 0x65, 0xFE, 0x7B, 0x88};
uint8_t receiverMac2[] = {0x8C, 0x4B, 0x14, 0x3B, 0x54, 0xE0};

// Define variables to store incoming message
esp_now_peer_info_t peerInfo1, peerInfo2;
typedef struct struct_message {
  uint8_t sender;
  uint8_t command;
  char data[32];
} struct_message;

struct_message incomingMessage;
int Enable_pin = 4;
bool messageSent = false;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // Serial.print("Last Packet Send Status: ");
  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingMessage, incomingData, sizeof(incomingMessage));
  // Parse and process the received data
  uint8_t senderCommand = incomingMessage.sender;
  uint8_t receivedCommand = incomingMessage.command;
  String receivedData = String(incomingMessage.data);

  Serial.print(incomingMessage.data);
  Serial.print(incomingMessage.command);
  Serial.println(incomingMessage.sender);
  /*Serial.print("Received Data: ");
  Serial.println(receivedData);
  Serial.print("Received Command: ");
  Serial.println(receivedCommand);*/
  // Perform actions based on receivedCommand and receivedData here
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

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

  // Register peers
  memcpy(peerInfo1.peer_addr, receiverMac1, 6);
  peerInfo1.channel = 0;
  peerInfo1.encrypt = false;

  memcpy(peerInfo2.peer_addr, receiverMac2, 6);
  peerInfo2.channel = 0;
  peerInfo2.encrypt = false;

  // Add peers
  if (esp_now_add_peer(&peerInfo1) != ESP_OK || esp_now_add_peer(&peerInfo2) != ESP_OK) {
    Serial.println("Failed to add peers");
    return;
  }

  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);

  pinMode(Enable_pin, OUTPUT);
  digitalWrite(Enable_pin, HIGH); // (HIGH to send to slave)
  Serial.println();
}

// Function to send a command and data to the receivers
void sendCommandAndData(uint8_t command, const String &data) {
  struct_message message;
  message.command = command;
  data.toCharArray(message.data, sizeof(message.data));
  esp_err_t result;

  // Send to receiver 1
  result = esp_now_send(receiverMac1, (uint8_t *)&message, sizeof(message));
  if (result != ESP_OK) {
    Serial.println("Error sending data to receiver 1");
  }

  // Send to receiver 2
  result = esp_now_send(receiverMac2, (uint8_t *)&message, sizeof(message));
  if (result != ESP_OK) {
    Serial.println("Error sending data to receiver 2");
  }
}

void loop() {
  // Example: Send command number 42 and a string as data
  sendCommandAndData(42, "All clear!");
  delay(10000);
}
