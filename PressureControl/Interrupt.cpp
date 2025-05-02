#include "SerialComms.h"
#include <Arduino.h>
#include "ArduinoPfieffer.h"

extern ArduinoPfieffer pfieffer;

bool running;
volatile unsigned long last_interrupt_time = 0;

void turn_off_Alicat() {
  float setPoint = 0.0;
  Serial.println("Setpoint to 0");
  ALICATSerial_MFC.listen();
  String command = "AS " + String(setPoint) + "\r";
  ALICATSerial_MFC.print(command);
  delayMicroseconds(100);
  readAndProcess(ALICATSerial_MFC);
}

void turn_on_pump() {
  Serial.println("Turning on pump");

  ASCII_char cmd = pfieffer.control_request("100", "111111");

  RS485Serial_PUMP.listen();
  RS485Mode_PUMP(WRITE);
  RS485Serial_PUMP.print(cmd);
  RS485Mode_PUMP(READ);
  delay(100);
  readAndProcess(RS485Serial_PUMP);

  pfieffer.free_message(cmd);
}

void turn_off_pump() {
  Serial.println("Turning off pump");

  ASCII_char cmd = pfieffer.control_request("100", "000000");

  RS485Serial_PUMP.listen();
  RS485Mode_PUMP(WRITE);
  RS485Serial_PUMP.print(cmd);
  RS485Mode_PUMP(READ);
  delayMicroseconds(100);
  readAndProcess(RS485Serial_PUMP);

  pfieffer.free_message(cmd);
}

void turn_off() {
  Serial.println("Turning Off");
  turn_off_Alicat();

  if (running) {
    turn_off_pump();
  } else {
    turn_on_pump();
  }

  running = !running;
}

void turn_off_wrapper() {
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 200) { // 200 ms debounce
    turn_off();
  }
  last_interrupt_time = interrupt_time;
}
