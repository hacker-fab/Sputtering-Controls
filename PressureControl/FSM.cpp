#include "FSM.h"
#include "SerialComms.h"
#include "Pressure.h"
#include "PID.h"
#include "DisplayPressure.h"
#include <Arduino.h>
#include "ArduinoPfieffer.h"

extern ArduinoPfieffer pfieffer;
extern ArduinoPfieffer pfieffer_gauge;

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

void send_and_process(ASCII_char command, SoftwareSerial& serial, ArduinoPfieffer& device) {
  serial.listen();
  RS485Mode_PUMP(WRITE);
  serial.print(command);
  RS485Mode_PUMP(READ);
  delay(100);
  readAndProcess(serial);
  device.free_message(command);
}

void handleTurnOn() {
  Serial.println("Turning on");
  ASCII_char cmd = pfieffer.control_request("010", "111111");
  //Serial.print("Turn on command sent: ");
  //Serial.println(cmd);
  send_and_process(cmd, RS485Serial_PUMP, pfieffer);
  queryState = (++turn_on_try < 1) ? TURN_ON : STANDBY_SET;
}

void handleStandbySet() {
  Serial.println("Setting standby mode");
  ASCII_char cmd = pfieffer.control_request("717", "010000");
  Serial.print("Standby command sent: ");
  Serial.println(cmd);
  send_and_process(cmd, RS485Serial_PUMP, pfieffer);
  queryState = SPEED_SET;
}

void handleSpeedSet() {
  char data[7];
  sprintf(data, "%06d", (int)(currSetPoint * 100));
  Serial.println("Setting pump speed");
  ASCII_char cmd = pfieffer.control_request("707", data);
  Serial.print("Speed set command sent: ");
  Serial.println(cmd);
  send_and_process(cmd, RS485Serial_PUMP, pfieffer);
  queryState = increasing_speed ? GAUGE : EVAC;
  if (increasing_speed) stage = 2;
}

void handleEvac() {
  Serial.println("Beginning evacuation");

  ASCII_char cmd1 = pfieffer.control_request("002", "111111");
  //Serial.print("Evac ON command sent: ");
  //Serial.println(cmd1);
  send_and_process(cmd1, RS485Serial_PUMP, pfieffer);

  delay(2000);

  ASCII_char cmd2 = pfieffer.control_request("026", "001");
  //Serial.print("Evac SETPOINT command sent: ");
  //Serial.println(cmd2);
  send_and_process(cmd2, RS485Serial_PUMP, pfieffer);

  queryState = GAUGE;
  stage = 2;
}

void handleSputterSpeed(pressure_measurement desired_pressure) {
  Serial.println("Setting sputter speed");

  ASCII_char cmd1 = pfieffer.control_request("002", "000000");
  //Serial.print("Sputter OFF command sent: ");
  //Serial.println(cmd1);
  send_and_process(cmd1, RS485Serial_PUMP, pfieffer);

  delay(2000);

  ASCII_char cmd2 = pfieffer.control_request("026", "011");
  //Serial.print("Sputter SETPOINT command sent: ");
  //Serial.println(cmd2);
  send_and_process(cmd2, RS485Serial_PUMP, pfieffer);

  reached_equilibrium(0.0, true, desired_pressure.exp);
  queryState = GAUGE;
  stage = 2;
}

void handleAlicat(pressure_measurement desired_pressure) {
  Serial.println("Setting Alicat setpoint");
  String command = "AS " + String(currSetPoint) + "\r";
  //Serial.print("Alicat command sent: ");
  //Serial.println(command);

  ALICATSerial_MFC.listen();
  ALICATSerial_MFC.print(command);
  delay(100);
  readAndProcess(ALICATSerial_MFC);
  reached_equilibrium(0.0, true, desired_pressure.exp);
  queryState = GAUGE;
}

void handleGauge(pressure_measurement desired_pressure) {
  RS485Serial_GAUGE.listen();
  Serial.println("Querying pressure gauge");

  ASCII_char cmd = pfieffer_gauge.data_request("740");
  //Serial.print("Gauge command sent: ");
  //Serial.println(cmd);

  RS485Mode_GAUGE(WRITE);
  RS485Serial_GAUGE.print(cmd);
  RS485Mode_GAUGE(READ);
  delay(100);
  pfieffer_gauge.free_message(cmd);

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
