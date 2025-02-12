#include <ModbusRTUMaster.h>
#include <SoftwareSerial.h>

// Define TX and RX pins for SoftwareSerial
const uint8_t TX_pin = 11;
const uint8_t RX_pin = 10;
const uint8_t CTS_pin = 9;
const uint8_t RTS_pin = 8;
SoftwareSerial modbusSerial(RX_pin, TX_pin);
ModbusRTUMaster modbus(modbusSerial, RTS_pin, CTS_pin);

const uint32_t baud = 9600;
const uint8_t ALICAT_ADDRESS = 1;

uint16_t pressure_reading[2];
uint16_t alarm;

void setup() {
    Serial.begin(9600);
    modbus.begin(baud, SERIAL_8N1); // Start Modbus communication
    pressure_reading[0] = 1;
}

void loop() {
    Serial.println("Communicating with Alicat...");


    delay(10); // Small delay to ensure stability

    uint16_t holdingRegisters[2];
    uint8_t error = modbus.readInputRegisters(ALICAT_ADDRESS, 1295, holdingRegisters, 2);
    Serial.print("Success of Read: ");
    Serial.println(error);


    digitalWrite(RTS_pin, HIGH);
    /*
    // Writing a holding register (Ensure register 999 is writable)
    uint8_t x = modbus.writeSingleHoldingRegister(ALICAT_ADDRESS, 999, 222);
    if (modbus.writeSingleHoldingRegister(ALICAT_ADDRESS, 999, 222) != 0) {
        Serial.println("Failed to write register 999");
    }
    Serial.print("Success of write ");
    Serial.println(x);


    if (modbus.writeSingleHoldingRegister(ALICAT_ADDRESS, 1000, 333) != 0) {
        Serial.println("Failed to write register 1000");
    }

    // Reading input registers (Check if register 999 exists)
    if (modbus.readInputRegisters(ALICAT_ADDRESS, 999, pressure_reading, 2) == 0) {
        Serial.print("Pressure Reading 1: ");
        Serial.println(pressure_reading[0]);
        Serial.print("Pressure Reading 2: ");
        Serial.println(pressure_reading[1]);
    } else {
        Serial.println("Failed to read input registers!");
    }
    */

    delay(1000); // Add a delay to prevent spamming requests
}
