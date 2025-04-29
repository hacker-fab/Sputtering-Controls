#include "FSM.h"
#include "SerialComms.h"
#include "Pressure.h"
#include "PID.h"
#include "DisplayPressure.h"
#include <Arduino.h>
#include <ArduinoPfiefferVacuum.h>

extern ArduinoPfiefferVacuum pfieffer;  // Assume global instance exists

const unsigned long interval = 5000;
const unsigned long sample_interval = 2000;

enum QueryState { TURN_ON, SPEED_SET, STANDBY_SET, EVAC, SPUTTER_SPEED, GAUGE, ALICAT };
QueryState queryState = TURN_ON;

int stage = 1;
int turn_on_try = 0;
int set_speed_try = 0;
unsigned long lastActionTime = 0;
bool in_evac = true;

float currSetPoint = 0;
String curr_sputter_speed = "003000";
bool increasing_speed = false;
int increase_increment = 10;

void increase_sputter_speed() {
  float value = curr_sputter_speed.toInt() / 100.0;
  value += increase_increment;
  if (value > 100) return;
  int intVal = round(value * 100);
  char buf[7];
  sprintf(buf, "%06d", intVal);
  curr_sputter_speed = String(buf);
}

void send_and_process(ASCII_char command, HardwareSerial& serial) {
  RS485Serial_PUMP.listen();
  RS485Mode_PUMP(WRITE);
  serial.print(command);
  RS485Mode_PUMP(READ);
  delay(100);
  readAndProcess(serial);
  pfieffer.free_message(command);
}

void handleTurnOn() {
  ASCII_char cmd = pfieffer.control_request("100", "111111");
  send_and_process(cmd, RS485Serial_PUMP);
  queryState = (++turn_on_try < 1) ? TURN_ON : STANDBY_SET;
}

void handleStandbySet() {
  ASCII_char cmd = pfieffer.control_request("717", "0000");
  send_and_process(cmd, RS485Serial_PUMP);
  queryState = SPEED_SET;
}

void handleSpeedSet() {
  char data[7];
  sprintf(data, "%06d", (int)(currSetPoint * 100));
  ASCII_char cmd = pfieffer.control_request("707", data);
  send_and_process(cmd, RS485Serial_PUMP);
  queryState = increasing_speed ? GAUGE : EVAC;
  if (increasing_speed) stage = 2;
}

void handleEvac() {
  ASCII_char cmd1 = pfieffer.control_request("002", "111111");
  send_and_process(cmd1, RS485Serial_PUMP);
  delay(2000);
  ASCII_char cmd2 = pfieffer.control_request("026", "001");
  send_and_process(cmd2, RS485Serial_PUMP);
  queryState = GAUGE;
  stage = 2;
}

void handleSputterSpeed(pressure_measurement desired_pressure) {
  ASCII_char cmd1 = pfieffer.control_request("002", "000000");
  send_and_process(cmd1, RS485Serial_PUMP);
  delay(2000);
  ASCII_char cmd2 = pfieffer.control_request("026", "011");
  send_and_process(cmd2, RS485Serial_PUMP);
  reached_equilibrium(0.0, true, desired_pressure.exp);
  queryState = GAUGE;
  stage = 2;
}

void handleAlicat(pressure_measurement desired_pressure) {
  ALICATSerial_MFC.listen();
  String command = "AS " + String(currSetPoint) + "\r";
  ALICATSerial_MFC.print(command);
  delay(100);
  readAndProcess(ALICATSerial_MFC);
  reached_equilibrium(0.0, true, desired_pressure.exp);
  queryState = GAUGE;
}

void handleGauge(pressure_measurement desired_pressure) {
  RS485Serial_GAUGE.listen();
  RS485Mode_GAUGE(WRITE);
  RS485Serial_GAUGE.write("0020074002=?107\r");
  RS485Mode_GAUGE(READ);
  delay(100);
  pressure_measurement measured_pressure = readAndProcess(RS485Serial_GAUGE);

  if (measured_pressure.exp == 0.0 && measured_pressure.frac == 0.0) {
    Serial.println("Failed Pressure Read");
    queryState = GAUGE;
    return;
  }

  displayPressure(desired_pressure, measured_pressure);
  Serial.print("Current Pressure: ");
  Serial.print(measured_pressure.frac);
  Serial.print("e");
  Serial.println(measured_pressure.exp);

  if (reached_equilibrium(measured_pressure.pressure, false, desired_pressure.exp)) {
    if (in_evac) {
      stage = 1;
      in_evac = false;
      queryState = SPUTTER_SPEED;
    } else if (measured_pressure.pressure < desired_pressure.exp && currSetPoint == 0.0) {
      stage = 1;
      increasing_speed = true;
      increase_sputter_speed();
      queryState = SPEED_SET;
    } else {
      currSetPoint = calculate_adjustment(desired_pressure, measured_pressure, currSetPoint, sample_interval);
      queryState = ALICAT;
    }
  } else {
    queryState = GAUGE;
  }
}

void updateFSM(pressure_measurement desired_pressure) {
  unsigned long now = millis();

  if (stage == 1 && now - lastActionTime >= interval) {
    switch (queryState) {
      case TURN_ON: handleTurnOn(); break;
      case STANDBY_SET: handleStandbySet(); break;
      case SPEED_SET: handleSpeedSet(); break;
      case EVAC: handleEvac(); break;
      case SPUTTER_SPEED: handleSputterSpeed(desired_pressure); break;
      default: Serial.println("Unhandled state in Stage 1"); break;
    }
    lastActionTime = now;
  }

  if (stage == 2 && now - lastActionTime >= sample_interval) {
    switch (queryState) {
      case ALICAT: handleAlicat(desired_pressure); break;
      case GAUGE: handleGauge(desired_pressure); break;
      default: Serial.println("Unhandled state in Stage 2"); break;
    }
    lastActionTime = now;
  }
}



