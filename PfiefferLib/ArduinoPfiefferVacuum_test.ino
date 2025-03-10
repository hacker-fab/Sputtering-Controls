/*
  ArduinoPfiefferVacuum_Test.ino - Test file for ArduinoPfiefferVacuum Library.
  Created by Shayaan Gandhi, February 16, 2024.
*/

#include <ArduinoPfiefferVacuum.h>
#include <SoftwareSerial.h>

#define TX_PIN 10
#define RX_PIN 11

SoftwareSerial RS485Serial(RX_PIN, TX_PIN);
//Check arddress
ArduinoPfiefferVacuum vacuum("001");

void setup() {
    Serial.begin(9600);
    RS485Serial.begin(9600);
    Serial.println("Starting ArduinoPfiefferVacuum Library Test...");

    testCommandFormatting();
    /*
    testChecksum();
    testDataRequest();
    testControlRequest();
    testResponseParsing();
    testPumpControl();
    */
}

void loop() {
    // Nothing here, tests run in setup()
}

void testCommandFormatting() {
    Serial.println("\nTesting command formatting...");
    ASCII_char command = vacuum.pfieffer_command_format("10", "707", "02", "100");
    if (command) {
        Serial.print("Formatted command: ");
        Serial.println(command);
        vacuum.free_message(command);
    } else {
        Serial.println("Command formatting failed!");
    }
}

void testChecksum() {
    Serial.println("\nTesting checksum calculation...");
    ASCII_char checksum = vacuum.get_check_sum("00110070702100", 14);
    if (checksum) {
        Serial.print("Checksum: ");
        Serial.println(checksum);
        vacuum.free_check_sum(checksum);
    } else {
        Serial.println("Checksum calculation failed!");
    }
}

void testDataRequest() {
    Serial.println("\nTesting data request...");
    ASCII_char request = vacuum.data_request("397");
    if (request) {
        Serial.print("Data request command: ");
        Serial.println(request);
        vacuum.free_message(request);
    } else {
        Serial.println("Data request failed!");
    }
}

void testControlRequest() {
    Serial.println("\nTesting control request...");
    ASCII_char request = vacuum.control_request("026", "1");
    if (request) {
        Serial.print("Control request command: ");
        Serial.println(request);
        vacuum.free_message(request);
    } else {
        Serial.println("Control request failed!");
    }
}

void testResponseParsing() {
    Serial.println("\nTesting response parsing...");
    ASCII_char response = "00102602123";  // Mock response
    ASCII_char* parsed = vacuum.decrypt_response(response);
    if (parsed) {
        Serial.print("Address: "); Serial.println(parsed[0]);
        Serial.print("Parameter: "); Serial.println(parsed[1]);
        Serial.print("Data Length: "); Serial.println(parsed[2]);
        Serial.print("Data: "); Serial.println(parsed[3]);
        vacuum.free_decrypt_response(parsed);
    } else {
        Serial.println("Response parsing failed!");
    }
}

void testPumpControl() {
    Serial.println("\nTesting pump control functions...");
    Serial.println("Turning on pump...");
    vacuum.turn_on_pump(RS485Serial);
    delay(2000);
    Serial.println("Turning off pump...");
    vacuum.turn_off_pump(RS485Serial);
    delay(2000);
    Serial.println("Venting pump...");
    vacuum.vent_pump(RS485Serial);
}
