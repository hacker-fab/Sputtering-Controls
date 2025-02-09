#include <PfiefferPumpLibrary.h>
#include <SoftwareSerial.h>


#define RS485_TX 11  // SoftwareSerial TX pin
#define RS485_RX 10  // SoftwareSerial RX pin
#define RS485_direction 9 //Enable for read and write

const ASCII_char PFIEFFER_ADDRESS = ___;

SoftwareSerial RS485Serial(RS485_TX, RS485_RX);

void setup()
{
    pinMode(RS485_direction, OUTPUT);
    Serial.begin(9600);
    RS485Serial.begin(9600);
    RS485_Master_Receive();
}

void loop()
{
    RS485_Master_Transmit();
    RS485Serial.println(*data_request(address, "797"));
    RS485_Master_Receive();
    char *response = get_response()
    Serial.println(response);
    /*
    RS485_Master_Transmit();
    RS485Serial.println(*control_request(address, "797", "230"));
    response = get_response()
    Serial.println(response);
    */
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
