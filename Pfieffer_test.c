#include <SoftwareSerial.h>
#include <string.h>


#define RS485_TX 11  // SoftwareSerial TX pin
#define RS485_RX 10  // SoftwareSerial RX pin
#define RS485_direction 9 //Enable for read and write

//const char* PFIEFFER_ADDRESS = ___;

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
    char buf[32] = "9610030902=?112";  // Increase buffer size to prevent overflow
    strncat(buf, "\r", sizeof(buf) - strlen(buf) - 1);  // Append carriage return
    RS485Serial.println(buf);
    RS485_Master_Receive();
    String response = get_response(2000);
    Serial.print("hh: ");
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

String get_response(uint8_t time_limit)
{
    unsigned long startTime = millis();
    String response = "";  // Use String instead of char*

    while (millis() - startTime < time_limit) { // Wait for max `time_limit` ms
        if (RS485Serial.available()) {
            response = RS485Serial.readStringUntil('\n'); // Read response
            break;
        }
    }

    return response;  // Return String (safe & dynamic)
}