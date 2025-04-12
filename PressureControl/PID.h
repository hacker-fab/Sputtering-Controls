#pragma once
#include "Pressure.h"


float calculate_adjustment(pressure_measurement desired_pressure, pressure_measurement measured_pressure, float currSetPoint, const unsigned long sample_interval);
void debug_pressure(pressure_measurement desired_pressure, pressure_measurement measured_pressure);
