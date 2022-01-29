


/**
   --------------------------------------------------------------------------------------------------------------------
   Example sketch/program showing how to read data from more than one PICC to serial.
   --------------------------------------------------------------------------------------------------------------------
   This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid

   Example sketch/program showing how to read data from more than one PICC (that is: a RFID Tag or Card) using a
   MFRC522 based RFID Reader on the Arduino SPI interface.

   Warning: This may not work! Multiple devices at one SPI are difficult and cause many trouble!! Engineering skill
            and knowledge are required!

   @license Released into the public domain.

   Typical pin layout used:
   -----------------------------------------------------------------------------------------
               MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
               Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
   Signal      Pin          Pin           Pin       Pin        Pin              Pin
   -----------------------------------------------------------------------------------------
   RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
   SPI SS 1    SDA(SS)      ** custom, take a unused pin, only HIGH/LOW required *
   SPI SS 2    SDA(SS)      ** custom, take a unused pin, only HIGH/LOW required *
   SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
   SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
   SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15

*/

#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include "ESP8266.h"
#include <SoftwareSerial.h>

SoftwareSerial mySerial(3, 2); /* RX:D3, TX:D2 */

String SSID  =      "CAM-RFID";
String PASSWORD =   "CamRfid3250@";
String HOST_NAME =  "192.168.3.20"; //ip del pc de acceso
String UBICACION =  "PISCINA"; //maximo 13 LETRAS para la pantalla
String UBICACION_ =  "PISCINA"; //maximo 13 LETRAS para el sistema
String CLUB =  "CAM";

String cadena = "";
#define Marcador    "CamRfid3250@" 
#define HOST_PORT   (80)


ESP8266 wifi(mySerial);

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

constexpr uint8_t RST_PIN = 9;     // Configurable, see typical pin layout above
constexpr uint8_t SS_1_PIN = 10;   // Configurable, take a unused pin, only HIGH/LOW required, must be diffrent to SS 2
constexpr uint8_t SS_2_PIN = 8;    // Configurable, take a unused pin, only HIGH/LOW required, must be diffrent to SS 1

constexpr uint8_t NR_OF_READERS = 2;
byte ssPins[] = {SS_1_PIN, SS_2_PIN};

MFRC522 mfrc522[NR_OF_READERS];   // Create MFRC522 instance.

byte ActualUID[4]; //almacenará el código del Tag leído

/**
   Initialize.
*/
void setup() {

  lcd.init();                      // initialize the lcd
  // Print a message to the LCD.
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print(UBICACION);


  mySerial.begin(9600);
  Serial.begin(9600);
  mySerial.setTimeout(10000);

  //Verificamos si el ESP8266 responde
  mySerial.println("AT");
  if (mySerial.find("OK"))
    Serial.println("Respuesta AT correcto");
  else
    Serial.println("Error en ESP8266");

  //-----Configuración de red-------//Podemos comentar si el ESP ya está configurado

  //ESP8266 en modo estación (nos conectaremos a una red existente)
  mySerial.println("AT+CWMODE=1");
  if (mySerial.find("OK"))
    Serial.println("ESP8266 en modo Estacion");

  //Nos conectamos a una red wifi
  mySerial.println("AT+CWJAP=\"" + SSID + "\",\"" + PASSWORD + "\"");
  Serial.println("Conectandose a la red ...");
  mySerial.setTimeout(10000); //Aumentar si demora la conexion
  if (mySerial.find("OK"))
  {
    Serial.println("WIFI conectado");
    lcd.setCursor(14, 0);
    lcd.print("OK");

  }
  else
  {
    Serial.println("Error al conectarse en la red");
    lcd.setCursor(14, 0);
    lcd.print("NO");

  }


  mySerial.setTimeout(2000);
  //Desabilitamos las conexiones multiples
  mySerial.println("AT+CIPMUX=0");
  if (mySerial.find("OK"))
    Serial.println("Multiconexiones deshabilitadas");

  //------fin de configuracion-------------------


  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)

  SPI.begin();        // Init SPI bus

  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN); // Init each MFRC522 card
    Serial.print(F("Reader "));
    Serial.print(reader);
    Serial.print(F(": "));
    mfrc522[reader].PCD_DumpVersionToSerial();
  }


}

/**
   Main loop.
*/
void loop() {
  /*
    mySerial.println("AT+ CIFSR=?");
    mySerial.setTimeout(10000);
        while(mySerial.available()>0)
      {
          char c = mySerial.read();
          Serial.write(c);
          cadena.concat(c);  //guardamos la respuesta en el string "cadena"
      }

    Serial.println(cadena);
  */

  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    // Look for new cards

    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial()) {
      //      dump_byte_array(mfrc522[reader].uid.uidByte, mfrc522[reader].uid.size);

      for (byte i = 0; i < mfrc522[reader].uid.size; i++) {
        Serial.print(mfrc522[reader].uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522[reader].uid.uidByte[i], HEX);
        ActualUID[i] = mfrc522[reader].uid.uidByte[i];
      }

      String myString;

      myString += String(ActualUID[0], HEX);
      myString += String(ActualUID[1], HEX);
      myString += String(ActualUID[2], HEX);
      myString += String(ActualUID[3], HEX);

      lcd.setCursor(1, 1);
      lcd.print("Leido OK");
      delay(500);

      Serial.print(myString);
      
      Serial.print("INICIANDO ENVIO");

      mySerial.println("AT+CIPSTART=\"TCP\",\"" + HOST_NAME + "\",80");
      if ( mySerial.find("OK"))
      {
        Serial.println();
        Serial.println();
        Serial.println();
        Serial.println("ESP8266 conectado con el servidor...");

        // http://localhost/index.php?id=bd4bcd4e&club=CALC&ubicacion=GIMNASIO

        //Armamos el encabezado de la peticion http
        String peticionHTTP = "GET /marca/index.php?id=";
        peticionHTTP = peticionHTTP + String(myString) + "&club=" + String(CLUB) + "&ubicacion=" + String(UBICACION_) + " HTTP/1.1\r\n";
        peticionHTTP = peticionHTTP + "Host: " + String(HOST_NAME) + "\r\n\r\n";

        Serial.println(peticionHTTP);

        //Enviamos el tamaño en caracteres de la peticion http:
        mySerial.print("AT+CIPSEND=");
        mySerial.println(peticionHTTP.length());

        //esperamos a ">" para enviar la petcion  http
        if (mySerial.find(">")) // ">" indica que podemos enviar la peticion http
        {
          Serial.println("Enviando HTTP . . .");
          mySerial.println(peticionHTTP);
          if ( mySerial.find("SEND OK"))
          {
            Serial.println("Peticion HTTP enviada:");
            Serial.println();
            Serial.println(peticionHTTP);
            Serial.println("Esperando respuesta...");

            Serial.print(cadena);
            mySerial.println("AT+CIPCLOSE");

            if ( mySerial.find("OK"))
              Serial.println("Conexion finalizada");

            lcd.setCursor(1, 1);
            lcd.print("Enviado OK");
            delay(500);



            /*                   fin_respuesta=true;

                          boolean fin_respuesta=false;
                          long tiempo_inicio=millis();
                          cadena="";

                          while(fin_respuesta==false)
                          {
                              while(mySerial.available()>0)
                              {
                                  char c=mySerial.read();
                                  Serial.write(c);
                                  cadena.concat(c);  //guardamos la respuesta en el string "cadena"
                              }

                                Serial.print(cadena);
                                mySerial.println("AT+CIPCLOSE");
                                fin_respuesta=true;

                              //finalizamos si la respuesta es mayor a 500 caracteres
                              if(cadena.length()>500) //Pueden aumentar si tenen suficiente espacio en la memoria
                              {
                                Serial.println("La respuesta a excedido el tamaño maximo");

                                mySerial.println("AT+CIPCLOSE");
                                if( mySerial.find("OK"))
                                  Serial.println("Conexion finalizada");
                                fin_respuesta=true;
                              }

                              if((millis()-tiempo_inicio)>10000) //Finalizamos si ya han transcurrido 10 seg
                              {
                                Serial.println("Tiempo de espera agotado");
                                mySerial.println("AT+CIPCLOSE");
                                if( mySerial.find("OK"))
                                  Serial.println("Conexion finalizada");
                                fin_respuesta=true;
                              }

                              if(cadena.indexOf("CLOSED")>0) //si recibimos un CLOSED significa que ha finalizado la respuesta
                              {
                                Serial.println();
                                Serial.println("Cadena recibida correctamente, conexion finalizada");
                                fin_respuesta=true;
                              }
                          }

            */
          }
          else
          {
            Serial.println("No se ha podido enviar HTTP.....");
          }
        }
      }
      else
      {
        Serial.println("No se ha podido conectarse con el servidor");
      }

      lcd.setCursor(1, 1);
      lcd.print("               ");
      //-------------------------------------------------------------------------------

      delay(1000); //pausa de 10seg antes de conectarse nuevamente al servidor (opcional)







      // Halt PICC
      mfrc522[reader].PICC_HaltA();
      // Stop encryption on PCD
      mfrc522[reader].PCD_StopCrypto1();
    } //if (mfrc522[reader].PICC_IsNewC
  } //for(uint8_t reader
}

/**
   Helper routine to dump a byte array as hex values to Serial.
*/
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
