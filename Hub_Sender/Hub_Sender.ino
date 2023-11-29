<Firebase_ESP_Client.h>
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


// Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
} 
else {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Assign the api key (required) /
  config.api_key = API_KEY;
  // Assign the RTDB URL (required) /
  config.database_url = DATABASE_URL;
  // Sign up /
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  }
}

void resetWiFiConnection() {
  // Disconnect from the current Wi-Fi network and clear stored credentials
  WiFi.disconnect(true);
}
// Function to send a command and data to the receiver
void sendCommandAndData(uint8_t command, const String& data) {
  struct_message message;
  message.command = command;
  data.toCharArray(message.data, sizeof(message.data));
  esp_err_t result = esp_now_send(receiverMac, (uint8_t *)&message, sizeof(message));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }
}

void loop() {
  
if (espNowMode) {
  sendCommandAndData(0, "All Good!");
  delay(10000); // Send every 10 seconds
}
else {
    if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    // Write an Int number on the database path test/int
    for (int i = 0; i < 2; i++) {
        String test = String(incomingMessage.command);
        test += "/UserID";
        test += String(count);
      if (Firebase.RTDB.set(&fbdo, test, incomingMessage.data)){
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
        count++;
      }
    else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      } 
  } 
  }
}

espNowMode = !espNowMode;
resetWiFiConnection();
setup();
}
