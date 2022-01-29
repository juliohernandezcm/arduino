#include <SPI.h>
#include <MFRC522.h>

constexpr uint8_t RST_PIN = 9;     // Configurable, see typical pin layout above
constexpr uint8_t SS_1_PIN = 10;   // Configurable, take a unused pin, only HIGH/LOW required, must be diffrent to SS 2
constexpr uint8_t SS_2_PIN = 8;    // Configurable, take a unused pin, only HIGH/LOW required, must be diffrent to SS 1

constexpr uint8_t NR_OF_READERS = 2;

byte ssPins[] = {SS_1_PIN, SS_2_PIN};

MFRC522 mfrc522[NR_OF_READERS];   // Create MFRC522 instance.

byte ActualUID[4]; //almacenará el código del Tag leído

bool lectura;
long previousMillis;
const int ledPIN = 5;
const int ledPIN2 = 6;

void setup() {

  Serial.begin(9600); // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)

  SPI.begin();        // Init SPI bus

  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN); // Init each MFRC522 card
   // mfrc522[reader].PCD_DumpVersionToSerial();
  }

 pinMode(6,OUTPUT);
 pinMode(7,OUTPUT);
 pinMode(ledPIN , OUTPUT);
 pinMode(ledPIN2 , OUTPUT);

  lectura = false;

 //digitalWrite(11,HIGH);
 //digitalWrite(12,HIGH);
 
}

void loop() {
  
  if (lectura) 
  {
      for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
      // Look for new cards
  
      if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial()) {
  //      dump_byte_array(mfrc522[reader].uid.uidByte, mfrc522[reader].uid.size);

        for (byte i = 0; i < mfrc522[reader].uid.size; i++) {
            //  Serial.print(mfrc522[reader].uid.uidByte[i] < 0x10 ? " 0" : " ");
            //  Serial.print(mfrc522[reader].uid.uidByte[i], HEX);   
              ActualUID[i]=mfrc522[reader].uid.uidByte[i];          
      } 

      String myString;
      
      myString += String(ActualUID[0],HEX);
      myString += String(ActualUID[1],HEX);
      myString += String(ActualUID[2],HEX);
      myString += String(ActualUID[3],HEX);
        
        Serial.print(myString + "/");
        
        // Halt PICC
        mfrc522[reader].PICC_HaltA();
        // Stop encryption on PCD
        mfrc522[reader].PCD_StopCrypto1();
        lectura = false;
        digitalWrite(ledPIN , LOW);
      } //if (mfrc522[reader].PICC_IsNewC
    } //for(uint8_t reader

    if (millis()-previousMillis > 10000) 
    {
      lectura = false;
      digitalWrite(ledPIN , LOW);
      Serial.print("NO/");
    }
    
  }
 else
 {
 //Mientras el puerto serie este accesible
 while (Serial.available())
 {
   byte dato = Serial.read();
 switch (dato){   
 case '0':
 digitalWrite(6,HIGH);
 delay(50);
 digitalWrite(6,LOW);
 //Serial.print("0");

 break;

 case '1':
 digitalWrite(7,HIGH);
 delay(50);
 digitalWrite(7,LOW);
 //Serial.print("1"); 

   break;
   
 case '2':
      lectura = true;
      digitalWrite(ledPIN , HIGH);
//      Serial.print("2");
      previousMillis = millis();

      
   break;

   case '3':
      digitalWrite(ledPIN2 , HIGH);
      delay(300);
      digitalWrite(ledPIN2 , LOW);
      delay(300);
      digitalWrite(ledPIN2 , HIGH);
      delay(300);
      digitalWrite(ledPIN2 , LOW);
      delay(300);
      digitalWrite(ledPIN2 , HIGH);
      delay(300);
      digitalWrite(ledPIN2 , LOW);
      delay(300);
      
   break;
 }
 }
 }
}
