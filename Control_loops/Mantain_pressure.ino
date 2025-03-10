/* Goal: Adjust Vacuum rotational speed and MFC such that is reaches desired pressure.
Idea Operations are highest rotation speed with lowest MFC*/



#include <ArduinoPfiefferVacuum.h>
#include <SoftwareSerial.h>

#define TX_PIN_PUMP 10
#define RX_PIN_PUMP 11
#define TX_PIN_ALICAT = 8;
#define RX_PIN_ALICAT = 9;

SoftwareSerial RS232Serial(RX_PIN_ALICAT, TX_PIN_ALICAT); // RX, TX
ModbusMaster node;

SoftwareSerial RS485Serial(RX_PIN_PUMP, TX_PIN_PUMP);
//Check arddress
ArduinoPfiefferVacuum vacuum("001");

void setup() {
    Serial.begin(9600);
    RS485Serial.begin(9600);
    vaccum.turn_on_pump(RS485Serial);
    vaccum.set_max_Vacuum_speed(RS485Serial);
}

void loop()
{
    //Keep increasing Alicat MFC until it reaches desired pressure
    node.writeMultipleRegisters(1299, 2)


}