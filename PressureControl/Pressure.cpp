#include "Pressure.h"
#include <Arduino.h>
#include <math.h>
#include <string.h>


#define N 25
#define Q 0.00005f  // = 5e-5 hPa = 0.5 millitorr
float pressureWindow[N];
int head = 0;
int count = 0;

pressure_measurement initializeDesiredPressure() {

  pressure_measurement desired_pressure;
  Serial.print("Desired pressure frac (hPa): ");
  while (!Serial.available());
  String frac = Serial.readStringUntil('\n');
  frac.trim();
  desired_pressure.frac = frac.toFloat();

  Serial.print("Desired pressure magnitude (hPa): ");
  while (!Serial.available());
  String exp = Serial.readStringUntil('\n');
  exp.trim();
  desired_pressure.exp = exp.toInt();

  desired_pressure.pressure = desired_pressure.frac * pow(10, desired_pressure.exp);
  return desired_pressure;
}

pressure_measurement pressure_conversion(char *unconverted_pressure) {
  if (strlen(unconverted_pressure) < 6) return {};
  char mantissa[5] = {0};
  char exp[3] = {0};
  strncpy(mantissa, unconverted_pressure, 4);
  strncpy(exp, unconverted_pressure + 4, 2);

  int frac = atoi(mantissa);
  int real_exp = atoi(exp) - 20;
  float frac_float = frac / 1000.0;

  pressure_measurement measured_pressure;
  measured_pressure.frac = frac_float;
  measured_pressure.exp = real_exp;
  measured_pressure.pressure = frac_float * pow(10, real_exp);
  return measured_pressure;
}

bool reached_equilibrium(float newPressure) {
  pressureWindow[head] = newPressure;
  head = (head + 1) % N;
  if (count < N) {
    count++;
    return false;
  }

  float sum = 0.0;
  for (int i = 0; i < N; i++) sum += pressureWindow[i];
  float mean = sum / N;

  float variance = 0.0;
  for (int i = 0; i < N; i++) {
    float diff = pressureWindow[i] - mean;
    variance += diff * diff;
  }
  float stdDev = sqrt(variance / N);
  return stdDev < Q;
}
