#include <SoftwareSerial.h>

SoftwareSerial SerialESP8266(3, 2); // RX, TX

void setup() { 
  Serial.begin(9600);
  //SerialESP8266.begin(9600); 
  SerialESP8266.begin(115200); 
}

void loop() { 
  if (SerialESP8266.available()) {
    Serial.write(SerialESP8266.read());
  }
  if (Serial.available()) {
    SerialESP8266.write(Serial.read());
  }
}
