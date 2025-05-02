#pragma once
#include "Pressure.h"

extern bool integral_active;

float calculate_adjustment(pressure_measurement desired_pressure, pressure_measurement measured_pressure, float currSetPoint, const unsigned long sample_interval);
void debug_pressure(pressure_measurement desired_pressure, pressure_measurement measured_pressure);
float proportional_coefficient(float normalized_error);
void fill_integral_buffer(pressure_measurement desired_pressure, pressure_measurement measured_pressure);
void init_coefficients(pressure_measurement desired_pressure);