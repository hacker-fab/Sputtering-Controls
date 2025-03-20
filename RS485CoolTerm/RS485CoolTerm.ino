#include <SoftwareSerial.h>

// Define TX and RX pins for SoftwareSerial
const uint8_t TX_pin = 2;
const uint8_t RX_pin = 3;
SoftwareSerial mySerial(RX_pin, TX_pin); // RX first, TX second

void setup() {
    Serial.begin(9600);   // Initialize hardware serial
    mySerial.begin(9600); // Initialize software serial
    Serial.println("RS485 Receiver Ready");
}

void loop() {
    // Check if data is available to read
    if (mySerial.available()) {
        char receivedChar = mySerial.read(); // Read one byte
        Serial.print("Received: ");
        Serial.println(receivedChar); // Print received data to Serial Monitor
    }
}
