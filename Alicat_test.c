#include <ModbusRTUMaster.h>
#include <SoftwareSerial.h>


//Look at alicat manual to determine TX and RX on Alicat. Below is for Arduino
const uint8_t TX_pin = 11;
const uint8_t RX_pin = 10;

SoftwareSerial modbusSerial(RX_pin, TX_pin);
ModbusRTUMaster modbus(modbusSerial);


const uint32_t baud = 19200;
//Page 29 of manual to set
const uint8_t ALICAT_ADDRESS = 1;


uint16_t pressure_reading[2];
uint16_t alarm;

void setup(){
    Serial.begin(9600);
    //Check Alicat doc to check second parameter
    modbus.begin(baud, SERIAL_8N1);
    pressure_reading[0] = 1;
}

void loop(){
    Serial.println("kk");
    modbus.writeSingleHoldingRegister(ALICAT_ADDRESS, 999, 222);
    modbus.writeSingleHoldingRegister(ALICAT_ADDRESS, 1000, 333);
    modbus.readInputRegisters(ALICAT_ADDRESS, 999, pressure_reading, 2);
    Serial.println(pressure_reading[0]);
    Serial.println(pressure_reading[1]);
    /*
    modbus.readInputRegisters(ALICAT_ADDRESS, 1295, alarm, 1);
    Serial.print("Alarm Active: ");
    Serial.println(alarm);
    modbus.readInputRegisters(ALICAT_ADDRESS, 1088, pressure_reading, 2);
    Serial.print("Pressure Reading: ");
    Serial.println(pressure_reading[0]);
    */
}
