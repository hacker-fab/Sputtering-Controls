#include "PID.h"
#include <Arduino.h>
#include "Pressure.h"

// -------------------- PID Constants & Globals --------------------

float Ki = 0.05;

const int INTEGRAL_WINDOW = 10;
float integralBuffer[INTEGRAL_WINDOW] = {0};

float integralSum = 0;
float integral = 0;
int bufferIndex = 0;

float normalized_error = 0;
float lastError = 0;
bool integral_active = false;

// -------------------- PID Helper Functions --------------------

float proportional_coefficient(float normalized_error) {
  float baseKp = 0.5;
  float minKp = 0.05;
  float maxKp = 1.0;

  float dynamicKp = baseKp * (abs(normalized_error));
  return constrain(dynamicKp, minKp, maxKp);
}

// -------------------- Debug Function --------------------

void debug_pressure(pressure_measurement desired_pressure, pressure_measurement measured_pressure) {
  Serial.print("Desired Pressure frac: ");
  Serial.println(desired_pressure.frac);
  Serial.print("Desired Pressure exp: ");
  Serial.println(desired_pressure.exp);

  Serial.print("Measured Pressure frac: ");
  Serial.println(measured_pressure.frac);
  Serial.print("Measured Pressure exp: ");
  Serial.println(measured_pressure.exp);
}

// -------------------- Main PID Adjustment --------------------

float calculate_adjustment(pressure_measurement desired_pressure, pressure_measurement measured_pressure, float currSetPoint, const unsigned long sample_interval) {
  if (measured_pressure.frac == 0) return currSetPoint;

  debug_pressure(desired_pressure, measured_pressure);

  float scale = 1.0 / desired_pressure.pressure; // Prevent div by zero
  normalized_error = (desired_pressure.pressure - measured_pressure.pressure) * scale;
  float raw_error = desired_pressure.pressure - measured_pressure.pressure;

  Serial.print("Normalized Error: ");
  Serial.println(normalized_error);

  if (abs(normalized_error) < 0.01) return currSetPoint; // Close enough

  float Kp = proportional_coefficient(normalized_error);
  float proportional = Kp * normalized_error;

  static int integralFillCount = 0;

  if (abs(proportional) < 0.01) {
    // Enable integral accumulation
    if (!integral_active) {
      Serial.println("Enabling integral mode");
      integral_active = true;
      integralFillCount = 0;
      integralSum = 0;
      for (int i = 0; i < INTEGRAL_WINDOW; i++) integralBuffer[i] = 0;
    }

    // Fill integral buffer
    integralSum -= integralBuffer[bufferIndex];
    integralBuffer[bufferIndex] = normalized_error;
    integralSum += normalized_error;

    bufferIndex = (bufferIndex + 1) % INTEGRAL_WINDOW;
    if (integralFillCount < INTEGRAL_WINDOW) integralFillCount++;

    integral = (integralFillCount >= INTEGRAL_WINDOW) ? Ki * integralSum : 0;
  } else {
    // Disable and clear integral buffer
    if (integral_active) {
      Serial.println("Disabling integral mode and clearing buffer");
      integral_active = false;
      integralFillCount = 0;
      integralSum = 0;
      for (int i = 0; i < INTEGRAL_WINDOW; i++) integralBuffer[i] = 0;
    }
    integral = 0;
  }

  float adjustment = proportional + integral;
  if (abs(adjustment) < 0.01) adjustment = 0;

  currSetPoint += adjustment;
  currSetPoint = constrain(currSetPoint, 0.0f, 50.0f);

  Serial.print("Current Setpoint: ");
  Serial.println(currSetPoint);
  Serial.print("Adjustment: ");
  Serial.println(adjustment);

  return currSetPoint;
}
