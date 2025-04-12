#include "RS485.h"
#include "Pressure.h"
#include <Arduino.h>

#define SerialDEpin_PUMP 6
#define SerialREpin_PUMP 7
#define SerialDEpin_GAUGE 2
#define SerialREpin_GAUGE 3
const char endChar = '\r';

SoftwareSerial RS485Serial_PUMP(8, 9);
SoftwareSerial RS485Serial_GAUGE(4, 5);
SoftwareSerial ALICATSerial_MFC(13, 12);

#define READ 0
#define WRITE 1

#define sentenceSize 128
char sentence[sentenceSize];
int sentenceIndex = 0;

void initializeSerials() {
  RS485Serial_PUMP.begin(9600);
  delay(30);
  RS485Serial_GAUGE.begin(9600);
  delay(30);
  ALICATSerial_MFC.begin(9600);
  Serial.begin(9600);
}

void initializeRS485Pins() {
  pinMode(SerialDEpin_PUMP, OUTPUT);
  pinMode(SerialREpin_PUMP, OUTPUT);
  pinMode(SerialDEpin_GAUGE, OUTPUT);
  pinMode(SerialREpin_GAUGE, OUTPUT);
  RS485Mode_PUMP(READ);
  RS485Mode_GAUGE(READ);
}

void RS485Mode_PUMP(bool mode) {
  digitalWrite(SerialDEpin_PUMP, mode);
  digitalWrite(SerialREpin_PUMP, mode);
}

void RS485Mode_GAUGE(bool mode) {
  digitalWrite(SerialDEpin_GAUGE, mode);
  digitalWrite(SerialREpin_GAUGE, mode);
}

pressure_measurement readAndProcess(SoftwareSerial &ss) {
  pressure_measurement result = {};  // Always return something
  while (ss.available()) {
    char c = ss.read();
    if (c != endChar && sentenceIndex < sentenceSize - 1) {
      sentence[sentenceIndex++] = c;
    } else {
      sentence[sentenceIndex] = '\0';
      result = processSentence(sentence);
      sentenceIndex = 0;
    }
  }
  return result;
}

pressure_measurement processSentence(char* msg) {
  pressure_measurement result = {};  // Always return something
  if (strlen(msg) < 11) return result;
  char param[4] = {0};
  strncpy(param, msg + 5, 3);
  int paramValue = atoi(param);

  if (paramValue == 309) {
    char data[7] = {0};
    strncpy(data, msg + 10, 6);
    Serial.print("Pump speed: ");
    Serial.println(data);
  } else if (paramValue == 740) {
    char data[7] = {0};
    strncpy(data, msg + 10, 6);
    result = pressure_conversion(data);
  }
  return result;
}
