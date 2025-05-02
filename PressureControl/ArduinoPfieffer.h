#pragma once

/*
  ArduinoPfieffer.h - Library for formatting Pfieffer Vacuum pump commands.
  Created by Shayaan Gandhi, February 16, 2025.
*/

#include <stdint.h>

typedef char* ASCII_char;

class ArduinoPfieffer
{
public:
    ArduinoPfieffer(ASCII_char address);
    ~ArduinoPfieffer();

    ASCII_char pfieffer_command_format(ASCII_char write, ASCII_char param_nums, ASCII_char data_len, ASCII_char data);
    ASCII_char get_check_sum(char *without_checksum, uint8_t len);
    ASCII_char data_request(ASCII_char param_num);
    ASCII_char control_request(ASCII_char param_num, ASCII_char data);
    void free_message(ASCII_char message);

    char _carriage_return;

private:
    ASCII_char _address;
};
