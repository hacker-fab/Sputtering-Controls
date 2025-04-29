#include "DisplayPressure.h"
#include "Pressure.h"
#include <Arduino.h>

void displayPressure(pressure_measurement desired_pressure, pressure_measurement measured_pressure)
{
    Serial.print(F("DATA,"));
    Serial.print(millis());
    Serial.print(F(","));
    Serial.print(desired_pressure.pressure, 6); // 6 decimal places for precision
    Serial.print(F(","));
    Serial.println(measured_pressure.pressure, 6);
}
