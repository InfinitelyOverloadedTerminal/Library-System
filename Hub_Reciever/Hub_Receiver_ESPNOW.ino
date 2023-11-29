#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH THE MAC Address of your receiver
uint8_t receiverMac[] = {0xA0, 0xB7, 0x65, 0xFE, 0x7B, 0x88};

// Define variables to store incoming message


esp_now_peer_info_t peerInfo;

typedef struct struct_message {
  uint8_t command;
  char data[32];
} struct_message;

struct_message incomingMessage;
int Enable_pin = 4;
bool messageSent = false;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  
  memcpy(&incomingMessage, incomingData, sizeof(incomingMessage));
  // Parse and process the received data
  uint8_t receivedCommand = incomingMessage.command;
  String receivedData = String(incomingMessage.data);

  /*Serial.print("Received Command: ");
  Serial.println(receivedCommand);
  Serial.print("Received Data: ");
  Serial.println(receivedData);*/
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
  
  // Register peer
  memcpy(peerInfo.peer_addr, receiverMac, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);

  pinMode(Enable_pin, OUTPUT);
  digitalWrite(Enable_pin, HIGH); // (HIGH to send to slave)

}

// Function to send a command and data to the receiver
void sendCommandAndData(uint8_t command, const String& data) {
  struct_message message;
  message.command = command;
  data.toCharArray(message.data, sizeof(message.data));
  esp_err_t result = esp_now_send(receiverMac, (uint8_t *)&message, sizeof(message));

  /*if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }*/
}

void loop() {
  // Example: Send command number 42 and a string as data
  
  if(!messageSent)
  {
    sendCommandAndData(42, "Hello! How are you!");
    Serial.println(incomingMessage.data);
    messageSent = true; 
  }
  delay(10000); // Send every 10 seconds
}