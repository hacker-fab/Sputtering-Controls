#include "SerialComms.h"
#include <Arduino.h>


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
  RS485Serial_PUMP.listen();
  RS485Mode_PUMP(WRITE);
  RS485Serial_PUMP.write("0011001006111111015\r");
  RS485Mode_PUMP(READ);
  delay(100);
  readAndProcess(RS485Serial_PUMP);
}

void turn_off_pump() {
  Serial.println("Turning off pump");
  RS485Serial_PUMP.listen();
  RS485Mode_PUMP(WRITE);
  //Turn off Pump
  RS485Serial_PUMP.write("0011001006000000009\r");
  RS485Mode_PUMP(READ);
  delayMicroseconds(100);
  readAndProcess(RS485Serial_PUMP);
}

void turn_off()
{
  Serial.println("Turning Off");
  turn_off_Alicat();
  if (running)
  {
    turn_off_pump();
  }
  else
  {
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
