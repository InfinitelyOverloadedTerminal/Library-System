#include <Arduino.h>
#include <Firebase_ESP_Client.h>

// Connecting to firebase
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Sean's Iphone"
#define WIFI_PASSWORD "12345678"

// Insert Firebase project API Key and RTDB URL
#define API_KEY "AIzaSyBqRdX8pYJ-eozkvlJK5r1IEImX_QezKqk"
#define DATABASE_URL "https://rfid-scanner-c7160-default-rtdb.firebaseio.com"

// Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;
int Enable_pin = 4;  

void setup() {
  Serial.begin(115200);
  pinMode(Enable_pin, OUTPUT);
  //The LOW makes sure that this will get stuff from the master.
  digitalWrite(Enable_pin, LOW); 

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Assign the API key (required)
  config.api_key = API_KEY;
  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;
  // Sign up
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("OK");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  // Assign the callback function for the long-running token generation task
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  pinMode(Enable_pin, OUTPUT);
  delay(10); 
  digitalWrite(Enable_pin, HIGH); 
}

void loop() {
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    
    // Read serial input
    while (Serial.available()) {
      String incomingData = Serial.readStringUntil('\n');
      Serial.println("Received Data: " + incomingData);

      // Write to the database
      String path = "Table " + String(count);
      if (Firebase.RTDB.set(&fbdo, path, incomingData)) {
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
        count++;
      } else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }
    }
  }
}

