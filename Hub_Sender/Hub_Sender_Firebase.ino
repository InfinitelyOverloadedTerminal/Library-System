#include <Arduino.h>
#include <Firebase_ESP_Client.h>

// Connecting to firebase
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "gabeDevice"
#define WIFI_PASSWORD "password"

// Insert Firebase project API Key and RTDB URL
//#define API_KEY "AIzaSyBqRdX8pYJ-eozkvlJK5r1IEImX_QezKqk"
//#define DATABASE_URL "https://rfid-scanner-c7160-default-rtdb.firebaseio.com"

#define API_KEY "AIzaSyAG2ogN6DiJ8EJbh5sTSwHCPPPIWXod3ZE"
#define DATABASE_URL "https://library-seating-system-default-rtdb.firebaseio.com/"

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


}

void loop() {
  
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 2000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    
    // Read serial input
    while (Serial.available()) {

      String incomingData = Serial.readStringUntil('\n');

      Serial.println("Received Data: " + incomingData);
      /*int buffer = incomingData.length() + 1;
      char char_array[buffer];
      incomingData.toCharArray(char_array, buffer);*/

      char seat_num = incomingData[incomingData.length()-2];
      incomingData[incomingData.length()-1] = '\0';
      incomingData[incomingData.length()-2] = '\0';
      //const char* empty = "EMPTY";
      Serial.println(incomingData.length());
      Serial.println(seat_num);
      //Serial.println(char_array);
      if(incomingData.compareTo("RESERVED") == 0)
      {
        String path = "Tables/Table 1/reserved";
        if (Firebase.RTDB.set(&fbdo, path, true)) 
        {
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
        }
        else 
        {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
        }
         
      }
      else if(incomingData.length() == 10)
      {
      // Write to the database
      
      //Serial.println(incomingData);
      //Serial.println(incomingData[incomingData.length()-1]);
      String path = "Tables/Table 1/Seat";
      path+=seat_num;
      
      //Serial.println(incomingData[incomingData.length()-1]);

      if (Firebase.RTDB.set(&fbdo, path, incomingData)) 
      {
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
      } else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }
      }
      else if(incomingData.compareTo("EMPTY") == 0)
      {
        String path = "Tables/Table 1/Seat";
        path+=seat_num;
        if (Firebase.RTDB.set(&fbdo, path, NULL)) {
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
        
      } else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }
      }
      
      else if(incomingData.compareTo("RELEASE") == 0)
      {
        String path = "Tables/Table 1/reserved";
        if (Firebase.RTDB.set(&fbdo, path, false)) {
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
        
      } else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }
      }
      else
      {
        Serial.println("Not working");
      }
    }
  }
}

