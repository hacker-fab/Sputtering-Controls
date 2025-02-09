#include <stdbool.h>
#include <stdlib.h>
#include <ParamaterFunc.h>
#include <PfiefferPumpLibrary.h>



bool pump_temp(ASCII_char address)
{
    RS485_Master_Transmit();
    //Data Request
    RS485Serial.println(*data_request(address, "305"));

    unsigned long startTime = millis();
    ASCII_char response = get_response(address, 2000);
    ASCII_char temp_param;
    ASCII_char data;
    ASCII_char *val= = decrypt_response(response);
    temp_param = val[0];
    data = val[2];
    if (get_param(*temp_param) == "OvTempPump" && atoi(*data) < 300 && atoi(*data) > 50) //Replace with actual pump temp range
    {
        return true;
    }
    return false;
}

