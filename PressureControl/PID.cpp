#include "PID.h"
#include <Arduino.h>
#include "Pressure.h"

float Kp = 1000, Ki = 0.0, Kd = 0;
float integralTerm = 0;
float error = 0;
float lastError = 0;

float calculate_adjustment(pressure_measurement desired_pressure, pressure_measurement measured_pressure, float currSetPoint, const unsigned long sample_interval){
  if (measured_pressure.frac == 0) return currSetPoint;

  debug_pressure(desired_pressure, measured_pressure);
  // PID Control
  error = desired_pressure.pressure - measured_pressure.pressure;
  if (abs(error) < 0.00001) return currSetPoint;

  float proportional = Kp * error;
  integralTerm += error * sample_interval;
  float integral = Ki * integralTerm;
  float derivative = (error - lastError) / sample_interval;
  float derivativeOutput = Kd * derivative;
  lastError = error;

  float adjustment = proportional + integral + derivativeOutput;
  if (abs(adjustment) < 0.01) adjustment = 0;

  currSetPoint += adjustment;
  if (currSetPoint > 50) currSetPoint = 50;
  else if (currSetPoint < 0) currSetPoint = 0;

  // Debug output
  Serial.print("Current Setpoint: "); Serial.println(currSetPoint);
  Serial.print("Adjustment: "); Serial.println(adjustment);
}

void debug_pressure(pressure_measurement desired_pressure, pressure_measurement measured_pressure)
{
  Serial.print("Desired Pressure frac: ");
    Serial.println(desired_pressure.frac);
    Serial.print("Desired Pressure exp: ");
    Serial.println(desired_pressure.exp);
    

    Serial.print("Measured Pressure frac: ");
    Serial.println(measured_pressure.frac);
    Serial.print("Measured Pressure exp: ");
    Serial.println(measured_pressure.exp);
    return;
}