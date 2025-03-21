#include <SoftwareSerial.h>

// Define TX and RX pins for SoftwareSerial
const uint8_t TX_pin = 2;
const uint8_t RX_pin = 3;
SoftwareSerial mySerial(RX_pin, TX_pin); // RX first, TX second

void setup() {
    Serial.begin(9600);   // Initialize hardware serial
    mySerial.begin(9600); // Initialize software serial
}


void loop() {
  //Check address: P[:797]; 2 for Thermal Evaporator
  mySerial.write("0020030902=?108\r");
  int x;
  while ((x = mySerial.read()) != '\r' && x != -1) {
      Serial.write(x);
      
      // Serial.print("Received from Software Serial: ");
      // Serial.println(data);
  }
  Serial.write("\r\n");
  delay(500);
}