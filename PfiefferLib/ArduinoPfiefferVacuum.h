/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/
#ifndef ArduinoPfiefferVacuum_h
#define ArduinoPfiefferVacuum_h

#include <Arduino.h>
#include <SoftwareSerial.h>


class ArduinoPfiefferVacuum
{
    typedef char *ASCII_char;
    public:
        ArduinoPfiefferVacuum(ASCII_char address, int TX_pin, int RX_pin);
        ASCII_char (ASCII_char address, ASCII_char write, ASCII_char param_nums, ASCII_char data_len, ASCII_char data);
        ASCII_char* decrypt_response(ASCII_char response);
        ASCII_char get_check_sum(char *without_checksum, uint8_t len);
        ASCII_char data_request(ASCII_char address, ASCII_char param_num);
        ASCII_char control_request(ASCII_char address, ASCII_char param_num, ASCII_char data);
        ASCII_char get_response(ASCII_char address, uint8_t time_limit);
        bool set_max_vacuum_speed(ASCII_char address);
        void maintain_strike_pressure(ASCII_char address);
        void maintain_sputter_pressure(ASCII_char address);
        bool turn_off_pump(ASCII_char address);
        bool turn_on_pump(ASCII_char address);
        bool vent_pump(ASCII_char address);
        void free_message(ASCII_char message);
        void free_check_sum(ASCII_char message);
        void free_decrypt_response(ASCII_char* response_array);
        char _carriage_return;
    private:
      ASCII_char _address;
};

#endif