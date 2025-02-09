#include <stdbool.h>
#include <stdlib.h>
#include <SoftwareSerial.h>
#include <string.h>

typedef char *ASCII_char;

//Need to implement error codes for most processes

ASCII_char pfieffer_command_format(ASCII_char address, ASCII_char write, ASCII_char param_nums, ASCII_char data_len, ASCII_char data)
{
    uint32_t comm_len = strlen(address) + strlen(write) + strlen(param_nums) + strlen(data_len) + strlen(data) + 4;
    ASCII_char buf = malloc(comm_len * sizeof(char));
    ASCII_char check_sum = get_check_sum();
    strncat(buf, address, write, param_nums, data_len, data, comm_len - 4);
    ASCII_char check_sum = get_check_sum(buf, comm_len - 4);
    buf = "";
    strncat(buf, address, write, param_nums, data_len, data, check_sum, carriage_return, comm_len);
    return buf;
    
}

ASCII_char* decrypt_response(ASCII_char response)
{
    ASCII_char param = malloc(4*sizeof(char));
    ASCII_char data_len = malloc(3*sizeof(char))
    //Get param
    strncpy(param, response + 5, 3);
    //Get data_len
    strncpy(data_len, response + 8, 2);

    ASCII_char data = malloc(atoi(data_len)*sizeof(char))
    //Get data
    strncpy(data, response + 10, atoi(data_len));

    ASCII_char* val_array[3];
    val_array[0] = param;
    val_array[1] = data_len;
    val_array[2] = data;
    return val_array;
}

ASCII_char get_check_sum(char *without_checksum, uint8_t len)
{
    uint8_t check_sum = 0;
    char sum_char[3];
    for (uint8_t i = 0; i < len; i++)
    {
        check_sum += *without_checksum[0];
    }
    sprintf(sum_char, "%d", check_sum)
    return &sum_char;
}
ASCII_char data_request(ASCII_char address, ASCII_char param_num)
{
    return pfieffer_command_format(address, "00", param_num, "02", "=?");
}

ASCII_char control_request(ASCII_char address, ASCII_char param_num, ASCII_char data)
{
    uint8_t data_len = strnlen(data);
    char data_len_char[2];
    sprintf(data_len_char, "%d", data_len)
    return pfieffer_command_format(address, "10", param_num, &data_len_char, data);
}

ASCII_char get_response(ASCII_char address, uint8_t time_limit)
{
    unsigned long startTime = millis();
    ASCII_char response = "";

    while (millis() - startTime < 2000) { // Wait for max 2 seconds
        if (RS485Serial.available()) {
            response = RS485Serial.readStringUntil('\n'); // Read response
            break;
        }
    }
    return response;
}

bool set_max_vaccum_speed(ASCII_char address)
{
    RS485_Master_Transmit();
    //Set rotation setting to %100
    RS485Serial.println(*control_request(address, "707", "100"));
    //Turn rotation setting on
    RS485Serial.println(*control_request(address, "026", "1"));
    //Implement error code
    return true
}

//Need to find out what pressue should be and what pump speed corresponds to that pressure
void maintain_strike_pressure(ASCII_char address)
{
    RS485_Master_Transmit();
    //Data Request for Set Rotational Speed in rpm
    RS485Serial.println(*data_request(address, "397"));
    return;
}

//Need to find out what pressue should be and what pump speed corresponds to that pressure
void maintain_sputter_pressure(ASCII_char address)
{
    RS485_Master_Transmit();
    //Data Request for Set Rotational Speed in rpm
    RS485Serial.println(*data_request(address, "397"));
    return;
}

bool turn_off_pump(ASCII_char address)
{
    RS485_Master_Transmit();
    //Control Request to turn off pump
    RS485Serial.println(*control_request(address, "023", "0"));
    RS485Serial.println(*control_request(address, "010", "0"));
    return true;
}

bool turn_on_pump(ASCII_char address)
{
    RS485_Master_Transmit();
    //Control Request to turn off pump
    RS485Serial.println(*control_request(address, "023", "1"));
    RS485Serial.println(*control_request(address, "010", "1"));
    return true;
}

bool vent_pump(ASCII_char address)
{
    RS485_Master_Transmit();
    //Venting mode set to direct
    RS485Serial.println(*control_request(address, "030", "2"));
    RS485Serial.println(*control_request(address, "012", "1"));
    return true;
}