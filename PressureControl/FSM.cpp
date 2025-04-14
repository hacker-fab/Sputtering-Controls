#include "FSM.h"
#include "RS485.h"
#include "Pressure.h"
#include "PID.h"
#include "DisplayPressure.h"
#include <Arduino.h>

enum QueryState { TURN_ON, SPEED_SET, STANDBY_SET, EVAC, SPUTTER_SPEED, GAUGE, ALICAT };
QueryState queryState = TURN_ON;

const unsigned long interval = 10000;
const unsigned long sample_interval = 2000;

int turn_on_try = 0;
int set_speed_try = 0;
int stage = 1;

float currSetPoint = 0;

unsigned long lastActionTime = 0;

void updateFSM(pressure_measurement desired_pressure) {
  unsigned long now = millis();

  if (stage == 1 && now - lastActionTime >= interval) {
    switch (queryState) {
      case TURN_ON:
        RS485Serial_PUMP.listen();
        RS485Mode_PUMP(WRITE);
        //Turn on
        RS485Serial_PUMP.write("0011001006111111015\r");
        RS485Mode_PUMP(READ);
        delay(100);
        readAndProcess(RS485Serial_PUMP);
        if (turn_on_try < 5) {
          queryState = TURN_ON;
          turn_on_try++;
        } else {
          queryState = EVAC;
        }
        break;

      case STANDBY_SET:
        RS485Serial_PUMP.listen();
        RS485Mode_PUMP(WRITE);
        //Set standby speed to 100
        RS485Serial_PUMP.write("0011071703100133\r");
        RS485Mode_PUMP(READ);
        delay(100);
        readAndProcess(RS485Serial_PUMP);
        queryState = SPEED_SET;
        break;

      case SPEED_SET:
        RS485Serial_PUMP.listen();
        RS485Mode_PUMP(WRITE);
        //Set set speed to 25
        RS485Serial_PUMP.write("0011070703025138\r");
        RS485Mode_PUMP(READ);
        delay(100);
        readAndProcess(RS485Serial_PUMP);
        queryState = EVAC;
        break;

      case EVAC:
        RS485Serial_PUMP.listen();
        RS485Mode_PUMP(WRITE);
        //Turn on Standby
        RS485Serial_PUMP.write("0011000206111111016\r");
        RS485Mode_PUMP(READ);
        delay(100);
        readAndProcess(RS485Serial_PUMP);
        delay(2000);
        RS485Mode_PUMP(WRITE);
        //Turn off set speed
        RS485Serial_PUMP.write("0011002603000125\r");
        RS485Mode_PUMP(READ);
        delay(100);
        readAndProcess(RS485Serial_PUMP);
        queryState = GAUGE;
        stage = 2;
        break;

      default: break;
    }
    lastActionTime = now;
  }

  if (stage == 2 && now - lastActionTime >= sample_interval) {
    switch (queryState) {
      case SPUTTER_SPEED:
        RS485Serial_PUMP.listen();
        RS485Mode_PUMP(WRITE);
        //Turn off standby
        RS485Serial_PUMP.write("0011000206000000010\r");
        RS485Mode_PUMP(READ);
        delay(100);
        readAndProcess(RS485Serial_PUMP);
        delay(2000);
        RS485Mode_PUMP(WRITE);
        //Turn on set speed
        RS485Serial_PUMP.write("0011002603111128\r");
        RS485Mode_PUMP(READ);
        delay(100);
        readAndProcess(RS485Serial_PUMP);
        queryState = GAUGE;
      case GAUGE:
        RS485Serial_GAUGE.listen();
        RS485Mode_GAUGE(WRITE);
        //Read pressure
        RS485Serial_GAUGE.write("0020074002=?107\r");
        RS485Mode_GAUGE(READ);
        delay(100);
        pressure_measurement measured_pressure = readAndProcess(RS485Serial_GAUGE);

        //Display Pressure
        displayPressure(desired_pressure, measured_pressure);
        if (reached_equilibrium(measured_pressure.pressure)) {
          currSetPoint = calculate_adjustment(desired_pressure, measured_pressure, currSetPoint, sample_interval);
          queryState = ALICAT;
        } else {
          queryState = GAUGE;
        }
        break;

      case ALICAT:
        ALICATSerial_MFC.listen();
        String command = "AS " + String(currSetPoint) + "\r";
        ALICATSerial_MFC.print(command);
        delay(100);
        readAndProcess(ALICATSerial_MFC);
        queryState = GAUGE;
        break;

      default: break;
    }
    lastActionTime = now;
  }
}
