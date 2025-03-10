#include <ModbusMaster.h>
#include <SoftwareSerial.h>

const uint8_t TX_pin = 2;
const uint8_t RX_pin = 3;

SoftwareSerial mySerial(RX_pin, TX_pin, true); // RX, TX
ModbusMaster node;
void setup() {
  // Modbus communication runs at 9600 baudrate
  Serial.begin(9600);
  mySerial.begin(9600);
  // Modbus slave ID 1
  node.begin(1, mySerial);
}
void loop()
{
  uint8_t result;
  uint16_t data[16];  
  // Read 16 registers starting at 0x3100, 05 quantities)
  //Note 0,5 returns 65535
  node.readInputRegisters(1315, 4);
  if (result == node.ku8MBSuccess)
   {
    Serial.print("ReadHoldingRegisters: ");
    Serial.println(node.getResponseBuffer(4));
    delay(1000);  
   }
  delay(1000);
