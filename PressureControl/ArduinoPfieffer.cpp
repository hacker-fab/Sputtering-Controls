/*
  ArduinoPfieffer.cpp - Library for formatting Pfieffer Vacuum pump commands.
  Created by Shayaan Gandhi, February 16, 2025.
*/

#include "ArduinoPfieffer.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <Arduino.h>

ArduinoPfieffer::ArduinoPfieffer(ASCII_char address)
{
    _address = address;
    _carriage_return = 13;
}

ArduinoPfieffer::~ArduinoPfieffer()
{
    // No dynamic members in class itself; nothing to free
}

ASCII_char ArduinoPfieffer::pfieffer_command_format(ASCII_char write, ASCII_char param_nums, ASCII_char data_len, ASCII_char data)
{
    // Calculate the total required buffer length:
    // base command + checksum (3 max digits) + '\r' + '\0'
    uint32_t comm_len = strlen(_address) + strlen(write) + strlen(param_nums)
                      + strlen(data_len) + strlen(data) + 3 + 1 + 1;

    ASCII_char buf = (ASCII_char)malloc(comm_len * sizeof(char));
    if (!buf) return NULL;

    // Build the base message
    buf[0] = '\0';
    strcat(buf, _address);
    strcat(buf, write);
    strcat(buf, param_nums);
    strcat(buf, data_len);
    strcat(buf, data);

    // Compute and append checksum
    ASCII_char check_sum = get_check_sum(buf, strlen(buf));
    strcat(buf, check_sum);
    strcat(buf, "\r");  // Append carriage return correctly
    free(check_sum);

    return buf;
}

ASCII_char ArduinoPfieffer::get_check_sum(char *without_checksum, uint8_t len)
{
    uint8_t check_sum = 0;
    for (uint8_t i = 0; i < len; i++) {
        check_sum += without_checksum[i];
    }

    ASCII_char sum_char = (ASCII_char)malloc(4 * sizeof(char));  // up to 3 digits + null
    if (!sum_char) {
        return NULL;
    }

    sprintf(sum_char, "%03d", check_sum);
    return sum_char;
}

ASCII_char ArduinoPfieffer::data_request(ASCII_char param_num)
{
    return pfieffer_command_format("00", param_num, "02", "=?");
}

ASCII_char ArduinoPfieffer::control_request(ASCII_char param_num, ASCII_char data)
{
    uint8_t data_len = strlen(data);
    char data_len_char[3];
    sprintf(data_len_char, "%02d", data_len);
    return pfieffer_command_format("10", param_num, data_len_char, data);
}

void ArduinoPfieffer::free_message(ASCII_char message)
{
    if (message) {
        free(message);
    }
}
