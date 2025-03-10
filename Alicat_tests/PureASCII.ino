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
    char a = 0x01;
    char b = 0x03;
    char c = 0x05;
    char d = 0x13;
    char e = 0x00;
    char f = 0x02;
    char g = 0x35;
    char h = 0x02;
    char data[] = {a, b, c, d, e, f, g, h};
    
    mySerial.write(""); 
    if (mySerial.available()) {
        char data = mySerial.read();
        Serial.print("Received from Software Serial: ");
        Serial.println(data);
    }
}


"""
RTU Protocol
Slave Address 2 bit | Function Code | Special Data | CRC

01 03 05 13 00 02 35 02


Functional Code
03 is Holding Register