#pragma once
#include <SoftwareSerial.h>
#include "Pressure.h"

extern SoftwareSerial RS485Serial_PUMP;
extern SoftwareSerial RS485Serial_GAUGE;
extern SoftwareSerial ALICATSerial_MFC;

#define READ 0
#define WRITE 1

void initializeSerials();
void initializeRS485Pins();
void RS485Mode_PUMP(bool mode);
void RS485Mode_GAUGE(bool mode);
pressure_measurement readAndProcess(SoftwareSerial &ss);
pressure_measurement processSentence(char* msg);
