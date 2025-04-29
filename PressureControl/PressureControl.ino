#include "SerialComms.h"
#include "Pressure.h"
#include "FSM.h"
#include "Interrupt.h"

#define interruptPin 2

pressure_measurement desired_pressure;

void printHelpMenu() {
  Serial.println(F("\n=== Help Menu ==="));
  Serial.println(F("This program runs a PI control loop to achieve a desired pressure"));
  Serial.println(F("inside the sputtering chamber."));
  Serial.println();
  Serial.println(F("Input Instructions:"));
  Serial.println(F("- Enter the decimal value and the exponent separately when prompted."));
  Serial.println(F("- Example: For 5.4e-4, enter:"));
  Serial.println(F("    Desired Pressure Decimal: 5.4"));
  Serial.println(F("    Desired Pressure Magnitude: -4"));
  Serial.println();
  Serial.println(F("Available Commands:"));
  Serial.println(F("  h : Show this help menu"));
  Serial.println(F("  s : Start the pressure control process"));
  Serial.println(F("  i : Manually trigger system interrupt (shutdown)"));
  Serial.println(F("=================\n"));
}

void setup() {
  running = true;
  initializeSerials();
  initializeRS485Pins();
  attachInterrupt(digitalPinToInterrupt(interruptPin), turn_off_wrapper, RISING);

  printHelpMenu();
  delay(100); // Give Serial time to flush before proceeding

  desired_pressure = initializeDesiredPressure();
}

void loop() {
  if (Serial.available() > 0) {
    char input = Serial.read();

    if (input == 'h' || input == 'H') {
      printHelpMenu();
      return;  // Prevent FSM update immediately after help
    }
    else if (input == 's' || input == 'S') {
      running = true;
      Serial.println(F("Starting pressure control..."));
    }
    else if (input == 'i' || input == 'I') {
      turn_off_wrapper();
    }
    // You can add more commands here if needed
  }

  if (running) {
    updateFSM(desired_pressure);
  }
}
