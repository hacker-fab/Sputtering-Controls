#include "DisplayPressure.h"
#include "Pressure.h"
#include <Arduino.h>

void displayPressure(pressure_measurement desired_pressure, pressure_measurement measured_pressure)
{
    Serial.print(measured_pressure.pressure);
    Serial.print('\t');
    Serial.println(desired_pressure.pressure);
}
