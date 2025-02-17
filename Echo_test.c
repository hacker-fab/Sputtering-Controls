#include <SoftwareSerial.h>

const uint8_t TX_pin = 11;
const uint8_t RX_pin = 10;

SoftwareSerial loopbackSerial(RX_pin, TX_pin);

void setup() {
    Serial.begin(9600);
    loopbackSerial.begin(19200);
    Serial.println("Echo test: Type in Serial Monitor.");
}

void loop() {
    if (Serial.available()) {
        char c = Serial.read();
        loopbackSerial.write(c);  // Send to TX
    }
    
    if (loopbackSerial.available()) {
        char c = loopbackSerial.read();
        Serial.print("Echo: ");
        Serial.println(c);  // Print what was received
    }
}
