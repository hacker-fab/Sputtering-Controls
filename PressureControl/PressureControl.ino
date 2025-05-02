#include "SerialComms.h"
#include "Pressure.h"
#include "FSM.h"
#include "Interrupt.h"
#include "PID.h"
#include "ArduinoPfieffer.h"

#define interruptPin 2

pressure_measurement desired_pressure;
ArduinoPfieffer pfieffer((ASCII_char)"001");
ArduinoPfieffer pfieffer_gauge((ASCII_char)"002");

void setup() {
  running = false;  // Start in idle mode until 's' received from Python
  initializeSerials();
  initializeRS485Pins();
  attachInterrupt(digitalPinToInterrupt(interruptPin), turn_off_wrapper, RISING);

  delay(100);  // Give Serial time to flush before proceeding
  waitForPressureInput(desired_pressure);  // Wait for pressure input from Python
  init_coefficients(desired_pressure);
}

void loop() {
  if (Serial.available()) {
    char input = Serial.read();

    if (input == 's' || input == 'S') {
      running = true;
      Serial.println(F("Starting pressure control..."));
    }
    else if (input == 'i' || input == 'I') {
      turn_off_wrapper();
    }
  }

  if (running) {
    updateFSM(desired_pressure);
  }
}
