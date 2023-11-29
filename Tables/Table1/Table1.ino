//Libraries
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SPI.h>//https://www.arduino.cc/en/reference/SPI
#include <MFRC522.h>//https://github.com/miguelbalboa/rfid
#include <esp_now.h>
#include <WiFi.h>

//Constants
#define greenPin 27
#define bluePin 26
#define redPin 25
#define buttonPin 12
#define SS_PIN  5  // ESP32 pin GPIO5 
#define RST_PIN 27 // ESP32 pin GPIO27 

LiquidCrystal_I2C lcd(0x3F, 16, 2);
MFRC522 rfid(SS_PIN, RST_PIN);
uint8_t receiverMac[] = {0x8C, 0x4B, 0x14, 0x39, 0x42, 0x98};

esp_now_peer_info_t peerInfo;

typedef struct struct_message {
  uint8_t sender;
  uint8_t command;
  char data[32];
} struct_message;

int buttonState = 0;
bool ableToRun = false;
int mode = 1; //Mode 1 is check-in/check-out, Mode 2 is reserve
const int tableNumber = 1;
const int tableCapacity = 4;
int occupiedSeats = 0;
char normalMessage[20];
char tableNum[20];
char capacityNum[20];
char seatsNum[20];
char rfidArr[9];
char reservedBy[9];
char rfidMsg[20];
char occupants[tableCapacity][9];

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status != 0){
    delay(100);
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  struct_message incomingMessage;
  memcpy(&incomingMessage, incomingData, sizeof(incomingMessage));
  // Parse and process the received data
  uint8_t receivedCommand = incomingMessage.command;
  uint8_t receivedSender = incomingMessage.sender;
  String receivedData = String(incomingMessage.data);

  Serial.print("Received from Sender: ");
  Serial.println(receivedSender);
  Serial.print("Received Command: ");
  Serial.println(receivedCommand);
  Serial.print("Received Data: ");
  Serial.println(receivedData);
  // Perform actions based on receivedCommand and receivedData here
  if(!ableToRun && receivedSender == 0 && receivedCommand == 42) {
    ableToRun = true;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Table Number: ");
    lcd.print(tableNum);
  }
}

void displayReservationCleared() {
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Reservation");
	lcd.setCursor(0, 1);
	lcd.print("Cleared");
	digitalWrite(bluePin, LOW);
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, HIGH);
	return;
}

void sayHello() {
	lcd.clear();
	 lcd.setCursor(0, 0);
	lcd.print("Welcome: ");
	lcd.setCursor(0, 1);
	lcd.print(rfidArr);
	digitalWrite(bluePin, HIGH);
	digitalWrite(redPin, LOW);
	digitalWrite(greenPin, LOW);
	return;
}

void sayGoodbye() {
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Goodbye: ");
	lcd.setCursor(0, 1);
	lcd.print(rfidArr);
	digitalWrite(bluePin, HIGH);
	digitalWrite(redPin, LOW);
	digitalWrite(greenPin, LOW);
	return;
}

void sayCapacity() {
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Seats Occupied:");
	lcd.setCursor(0, 1);
	lcd.print(seatsNum);
	lcd.print("/");
	lcd.print(capacityNum);
	return;
}

void checkIn() {
	for (int i = 0; i <= tableCapacity; i++) {
	  //Serial.print("Checking i=");
	  //Serial.print(i);
	  //Serial.print(" occupant is: ");
	  //Serial.println(occupants[i]);
	  if((strcmp(occupants[i], rfidArr)) == 0) {
      //Serial.print("Found match at ");
      //Serial.println(i);
      strcpy(occupants[i], "");
      sendCommandAndData(tableNumber, i+1, "EMPTY");
      sayGoodbye();
      occupiedSeats--;
      /*for (int j = i; j <= tableCapacity; j++) {
        //Serial.print("Checking j=");
        //Serial.print(j);
        //Serial.print(" occupant is: ");
        //Serial.println(occupants[j]);
        if((strcmp(occupants[j], "")) != 0) {
        //Serial.print("Found non blank entry at ");
        //Serial.println(j);
        strcpy(occupants[j-1], occupants[j]);
        }
      }
      */
      delay(1000);
      sprintf(seatsNum, "%d", occupiedSeats);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Seats Occupied:");
      lcd.setCursor(0, 1);
      lcd.print(seatsNum);
      lcd.print("/");
      lcd.print(capacityNum);
      digitalWrite(bluePin, LOW);
      digitalWrite(redPin, LOW);
      digitalWrite(greenPin, HIGH); 
      return;
    }
  }
	if(occupiedSeats >= tableCapacity) {
	  Serial.println("Table is full");
	  lcd.clear();
	  lcd.setCursor(0, 0);
	  lcd.print("This Table is");
	  lcd.setCursor(0, 1);
	  lcd.print("Completely Full");
	  digitalWrite(bluePin, LOW);
	  digitalWrite(redPin, HIGH);
	  digitalWrite(greenPin, LOW);
	  delay(1000);
	  digitalWrite(bluePin, LOW);
	  digitalWrite(redPin, HIGH);
	  digitalWrite(greenPin, LOW);
	  lcd.clear();
	  lcd.setCursor(0, 0);
	  lcd.print("Tap Card to");
	  lcd.setCursor(0, 1);
	  lcd.print("Check Out");
	  //goto start;
	} else {
    for (int i = 0; i <= tableCapacity; i++) {
        //Serial.print("Checking j=");
        //Serial.print(i);
        //Serial.print(" occupant is: ");
        //Serial.println(occupants[i]);
        if((strcmp(occupants[i], "")) == 0) {
        //Serial.print("Found blank entry at ");
        //Serial.println(j);
        strcpy(occupants[i], rfidArr);
        sayHello();
        occupiedSeats++;
        sendCommandAndData(tableNumber, i+1, rfidArr);
        delay(1000);
        sprintf(seatsNum, "%d", occupiedSeats);
        sayCapacity();
        digitalWrite(bluePin, LOW);
        digitalWrite(redPin, LOW);
        digitalWrite(greenPin, HIGH); 
        return;
        }
    }
	}
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init(); // init MFRC522
  //Serial.println("Tap an RFID/NFC tag on the RFID-RC522 reader");

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


  // begin lcd and write table number
  lcd.begin();
  lcd.backlight();

  //strcpy(normalMessage, "Table: ");
  sprintf(tableNum, "%d", tableNumber);
  sprintf(capacityNum, "%d", tableCapacity);
  sprintf(seatsNum, "%d", occupiedSeats);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Table Number: ");
  lcd.print(tableNum);
  //strncat(normalMessage, tableNum, 3);
  //lcd.print(normalMessage);

  // set pin modes
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  digitalWrite(greenPin, HIGH);
  digitalWrite(redPin, LOW);
  digitalWrite(bluePin, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  start:
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    if(mode == 1) {
      mode = 2;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Tap Card to");
      lcd.setCursor(0, 1);
      lcd.print("Reserve Table ");
      lcd.print(tableNum);
      digitalWrite(bluePin, HIGH);
      digitalWrite(redPin, LOW);
      digitalWrite(greenPin, LOW);
      delay(100);
    } else if(mode == 2) {
      mode = 1;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Tap Card to");
      lcd.setCursor(0, 1);
      lcd.print("Check-In or Out");
      digitalWrite(bluePin, LOW);
      digitalWrite(redPin, LOW);
      digitalWrite(greenPin, HIGH);
      delay(100);
    } else if(mode == 3) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Table Reserved");
      lcd.setCursor(0, 1);
      lcd.print("By: ");
      lcd.print(rfidArr);
      digitalWrite(bluePin, LOW);
      digitalWrite(redPin, HIGH);
      digitalWrite(greenPin, LOW);
    }
  }
  if (!ableToRun) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Await Encrypted");
    lcd.setCursor(0, 1);
    lcd.print("Hub Connection");
    digitalWrite(bluePin, HIGH);
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, HIGH);
    delay(500);
  } else {
    if (rfid.PICC_IsNewCardPresent()) { // new tag is available
      if (rfid.PICC_ReadCardSerial()) { // NUID has been readed
        MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
        //Serial.print("RFID/NFC Tag Type: ");
        //Serial.println(rfid.PICC_GetTypeName(piccType));
        memset(rfidArr, 0, sizeof(rfidArr));
        for (int cnt = 0; cnt < rfid.uid.size; cnt++)
          {
            // convert byte to its ascii representation
            sprintf(&rfidArr[cnt * 2], "%02X", rfid.uid.uidByte[cnt]);
          }
          Serial.println(rfidArr);
          if (mode == 1) {
            Serial.println("Mode 1");
            checkIn();
			goto start;
          } else if (mode == 2) {
            Serial.println("Mode 2");
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Table Reserved");
            lcd.setCursor(0, 1);
            lcd.print("By: ");
            lcd.print(rfidArr);
            digitalWrite(bluePin, LOW);
            digitalWrite(redPin, HIGH);
            digitalWrite(greenPin, LOW);
            sendCommandAndData(tableNumber, 0, "RESERVED");
            delay(1000);
            strncpy(reservedBy, rfidArr, sizeof(rfidArr));
            mode = 3;
            goto start;
          }
          else if (mode == 3) { //Reserved
            Serial.println("Mode 3");
            Serial.print("Reserved by: ");
            Serial.print(reservedBy);
            Serial.print(" and being scanned by: ");
            Serial.println(rfidArr);
            if((strcmp(reservedBy, rfidArr)) == 0) {
                displayReservationCleared();
                sendCommandAndData(tableNumber, 0, "RELEASE");
                strcpy(reservedBy, "");
                delay(1000);
                mode = 2;
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Tap Card to");
                lcd.setCursor(0, 1);
                lcd.print("Reserve Table ");
                lcd.print(tableNum);
                digitalWrite(bluePin, HIGH);
                digitalWrite(redPin, LOW);
                digitalWrite(greenPin, LOW);
            } else {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Table Already");
              lcd.setCursor(0, 1);
              lcd.print("Reserved");
              digitalWrite(bluePin, LOW);
              digitalWrite(redPin, HIGH);
              digitalWrite(greenPin, LOW);
            }
          }
        rfid.PICC_HaltA(); // halt PICC
        rfid.PCD_StopCrypto1(); // stop encryption on PCD
      }
    }
  }
}

// Function to send a command and data to the receiver
void sendCommandAndData(uint8_t sender, uint8_t command, const String& data) {
  struct_message message;
  message.command = command;
  message.sender = sender;
  data.toCharArray(message.data, sizeof(message.data));
  esp_err_t result = esp_now_send(receiverMac, (uint8_t *)&message, sizeof(message));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }
}

