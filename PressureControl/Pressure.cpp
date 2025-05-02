#include "Pressure.h"
#include <Arduino.h>
#include <math.h>
#include <string.h>


#define N 10
#define Q 0.1f
float pressureWindow[N];
int head = 0;
int count = 0;

void waitForPressureInput(pressure_measurement &desired_pressure) {
  Serial.println("WAITING_FOR_PRESSURE");

  String fracStr = "";
  String expStr = "";

  // Wait for "PSET,<frac>,<exp>"
  while (true) {
    if (Serial.available()) {
      String line = Serial.readStringUntil('\n');
      line.trim();
      if (line.startsWith("PSET,")) {
        int firstComma = line.indexOf(',');
        int secondComma = line.indexOf(',', firstComma + 1);
        if (secondComma != -1) {
          fracStr = line.substring(firstComma + 1, secondComma);
          expStr = line.substring(secondComma + 1);
          break;
        }
      }
    }
  }

  desired_pressure.frac = fracStr.toFloat();
  desired_pressure.exp = expStr.toInt();
  desired_pressure.pressure = desired_pressure.frac * pow(10, desired_pressure.exp);

  Serial.println("PRESSURE_RECEIVED");
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

bool reached_equilibrium(float newPressure, bool clear, float desired_magnitude) {
  if (clear)
  {
    for (int i = 0; i < N; i++)
    {
      pressureWindow[i] = i;
    }
    count = 0;
    return false;
  }
  int scale = desired_magnitude * -1;
  float scaled_pressure = newPressure * pow(10, scale);
  pressureWindow[head] = scaled_pressure;

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
  Serial.print("Current Standard Deivation: ");
  Serial.println(stdDev);
  return stdDev < Q;
}
