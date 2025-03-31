#include <SoftwareSerial.h>

//RS485 PUMP Serial
SoftwareSerial RS485Serial_PUMP(8, 9); // RX, TX
//RS485 GAUGE Serial
SoftwareSerial RS485Serial_GAUGE(4, 5); // RX, TX

//RS485 PUMP Communication control pins
#define SerialDEpin_PUMP 6
#define SerialREpin_PUMP 7

//RS485 Gauge Communication control pins
#define SerialDEpin_GAUGE 2
#define SerialREpin_GAUGE 3

// Values used for RS485 communication control
#define READ 0
#define WRITE 1

const char endChar = '\r';

#define sentenceSize 128
char sentence[sentenceSize];

// Helper function for changing both RS485 comm pins
// to receive or transmit
void RS485Mode_PUMP(bool mode) {
  digitalWrite(SerialDEpin_PUMP, mode);
  digitalWrite(SerialREpin_PUMP, mode);
}

void RS485Mode_GAUGE(bool mode) {
  digitalWrite(SerialDEpin_GAUGE, mode);
  digitalWrite(SerialREpin_GAUGE, mode);
}

void setup() {
  // Serial to RS485:
  RS485Serial_PUMP.begin(9600);
  RS485Serial_GAUGE.begin(9600);
  // Serial to USB for debugging
  Serial.begin(9600);

  //RS485 Communication control pins
  pinMode(SerialDEpin_PUMP, OUTPUT);
  pinMode(SerialREpin_PUMP, OUTPUT);
  pinMode(SerialDEpin_GAUGE, OUTPUT);
  pinMode(SerialREpin_GAUGE, OUTPUT);
  RS485Mode_PUMP(READ);
  RS485Mode_GAUGE(READ);
}


// Used to cycle through messages for Serial.write()
// It seems that Pfeiffer TC 110 only accepts one message per Serial.write()
// or the RS485 r/w timing is slightly off,
// thus queryTurn cycles through messages
int queryTurn = 0;

bool newData = false;


unsigned long lastCheckMillis = 0;
int interval = 2000;

void loop() {
  // Current runtime in ms
  // Sending of serial messages, LCD value refresh, and "blank" LCD (no new data)
  // depend on independent update intervals
  unsigned long currentMillis = millis();
  // Only one message can be sent at a time to the pump drive it seems
  // Also, less than or greater than ~20s delay seems to either not send the message
  // or not give enough time to receive
  if (currentMillis - lastCheckMillis > interval) {
    if (queryTurn == 0) {
      Serial.print("hello\n");
      //Turn Pump on
      RS485Mode_PUMP(WRITE);
      RS485Serial_PUMP.write("0010030902=?107");
      RS485Serial_PUMP.write(endChar);
      queryTurn++;
    } else if (queryTurn == 1) {
      //Read Pressure
      RS485Mode_GAUGE(WRITE);
      RS485Serial_GAUGE.write("0020074002=?107");
      RS485Serial_GAUGE.write(endChar);
      queryTurn = 0;
    } else if (queryTurn == 2) {
      //IGNORE RN
      /*
      //[P:316] DrvPower, 0010031602=?105\r
      RS485Serial.write("0010031602=?105");
      RS485Serial.write(endChar);
      //queryTurn = 0;
      */
    }

    // No delay gets good values
    RS485Mode_PUMP(READ);
    RS485Mode_GAUGE(READ);
    lastCheckMillis = currentMillis;
  }

  readSerial(currentMillis);
  if (newData)
  {
    int param_len = 3;
    // +1 for the null terminator
    char param[param_len + 1];              
    strncpy(param, sentence + 5, param_len);
    param[param_len] = '\0'; 
    if (atoi(param) == 309)
    {
      int data_len = 6;
      // +1 for the null terminator
      char data[data_len + 1];              
      strncpy(data, sentence + 10, data_len);
      data[data_len] = '\0'; 
      Serial.print("Pump speed: ");
      Serial.println(data);
    }
    else if (atoi(param) == 740)
    {
      int data_len = 6;
      // +1 for the null terminator
      char data[data_len + 1];              
      strncpy(data, sentence + 10, data_len);
      data[data_len] = '\0'; 
      Serial.print("Pressure: ");
      Serial.println(data);
    } 
    newData = false;
  }
}

void readSerial(unsigned long currentMillis) {
  RS485Mode_PUMP(READ);
  RS485Mode_GAUGE(READ);
  if (RS485Serial_PUMP.available() || RS485Serial_GAUGE.available()) {
    static int i = 0;
    char ch;
    if (RS485Serial_PUMP.available())
    {
      ch = RS485Serial_PUMP.read();
    }
    else {
      ch = RS485Serial_GAUGE.read();
    }
    //Serial.println((uint8_t)ch, HEX);
    if (ch != endChar && ch != -1 && i < sentenceSize - 1) {
      sentence[i] = ch;
      i++;
    } else if (ch == endChar || i == sentenceSize - 1) {
      sentence[i] = '\0';
      i++;
      while (i < sentenceSize) {
        sentence[i] = '\0';
        i++;
      }
      i = 0;
      newData = true;
    }
  }
}

// Sums ASCII values for chars in a char string
int asciisum(int st, int en) {
  int asciitotal = 0;
  for (int i = st; i <= en; i++) {
    asciitotal = asciitotal + sentence[i];
  }
  return asciitotal;
}

// Makes sure all parameters included in message through the data
// section equal the checksum value passed at the end of the received message
bool validCheckSum(char sentence[]) {
  char checksum[4];
  strncpy(checksum, sentence + 16, 3);
  //checksum[3] = '\0';
  int intchecksum = atoi(checksum);
  int modulo = asciisum(0, 15) % 256;
  return (intchecksum == modulo);
}