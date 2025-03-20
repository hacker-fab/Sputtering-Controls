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
    Serial.print("Change set point: "); // Prompt user
    
    while (!Serial.available()); // Wait for user input
    
    String input = Serial.readStringUntil('\n'); // Read input
    input.trim(); // Remove any extra spaces or newline characters
    
    String command = "AS " + input + "\r";
    mySerial.print(command); // Send formatted command over software serial
    
    Serial.print("Sent: ");
    Serial.println(command);
    Serial.print("\n");
}