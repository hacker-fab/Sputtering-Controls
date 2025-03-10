#include <stdbool.h>
#include <stdlib.h>
#include <SoftwareSerial.h>
#include <string.h>
#include <ParamaterFunc.h>
#include <PfiefferPumpLibrary.h>
#include <StandByLib.h>


/*
Standby Phase
Verify that the chamber is closed and that the pump temperature is within a safe range
Perform software checks and calibrations for the following devices:
-MFC
-Pressure gauge.
-QCM
-Radio transceiver
*/

/*
Set the pump to its maximum speed
Use closed-loop feedback from the pressure gauge to monitor and regulate the system until the PREVAC_PRESSURE is reached
*/

/*
Strike Phase
Set flow rates to STRIKE_ARFLOW and STRIKE_OFLOW using the MFCs
Use closed-loop feedback with the pump speed to maintain the STRIKE_PRESSURE
Activate the radio transceiver with an initial RF power of 5W
Increase the power by 1W every 20 seconds
Use SWR meter to detect plasma ignition
Once plasma is detected, start a timer for the sputtering process
*/

/*
Sputtering Phase
Reduce the flow rates of the MFCs to SPUTTER_ARFLOW SPUTTER_OFLOW
Adjust the pump speed using closed-loop feedback to achieve and maintain the SPUTTER_PRESSURE
Ramp up power to SPUTTER_POWER
Continue sputtering for the preset time duration
*/

/*
Cooldown Phase
Turn off the RF power and radio
Turn off the MFCs
Turn off the pump
Alert the user that the system is ready for venting
*/

/*
User Post-Operation Actions
Vent the pump to equalize the chamber pressure
Shut off water cooling
Remove the chip or sample
*/
typedef char *ASCII_char;

#define RS485_TX 11  // SoftwareSerial TX pin
#define RS485_RX 10  // SoftwareSerial RX pin
#define RS485_direction 9 //Enable for read and write

#define MODBUS_TX 8
#define MODBUS_RX 7

const char carriage_return = 13;
const uint8_t ALICAT_ADDRESS = 1;
const ASCII_char PFIEFFER_ADDRESS = ___;

SoftwareSerial RS485Serial(RS485_TX, RS485_RX);
SoftwareSerial modbusSerial(MODBUS_RX, MODBUS_TX);
ModbusRTUMaster modbus(modbusSerial);

void setup()
{
    pinMode(RS485_direction, OUTPUT);
    Serial.begin(9600);
    RS485Serial.begin(9600);
    RS485_Master_Receive();
    modbus.begin(baud, SERIAL_8N1);

    //Turn on pump
    turn_on_pump(PFIEFFER_ADDRESS);

}

//Will use interrupts to start procedures
void loop()
{

}

bool stand_by()
{
    //Check pump temperature
    bool pump = pump_temp(PFIEFFER_ADDRESS)
    //Check MFC for gas flow is active
    //Check Pressure gauage is active
    //Check QCM is active
    //Check Radio Transciever is active
    //Check is chmaber is sealed properly (perhaps look at the pressure of the chamber)
}


void RS485_Master_Receive()
{
    digitalWrite(RS485_direction, LOW);
    return;
}

void RS485_Master_Transmit()
{
    digitalWrite(RS485_direction, HIGH);
    return;
}


