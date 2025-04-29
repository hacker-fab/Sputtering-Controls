#pragma once

struct pressure_measurement {
  float pressure;
  float frac;
  float exp;
};


pressure_measurement initializeDesiredPressure();
pressure_measurement pressure_conversion(char* unconverted_pressure);
bool reached_equilibrium(float newPressure, bool clear, float desired_magnitude);

// Exposed global pressure readings
extern float measuredPressure;
extern float measuredPressure_frac;
extern int measuredPressure_exp;
