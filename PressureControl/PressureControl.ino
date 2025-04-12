#include "RS485.h"
#include "Pressure.h"
#include "FSM.h"

pressure_measurement desired_pressure;

void setup() {
  initializeSerials();
  initializeRS485Pins();
  desired_pressure = initializeDesiredPressure();
}

void loop() {
  updateFSM(desired_pressure);
}
