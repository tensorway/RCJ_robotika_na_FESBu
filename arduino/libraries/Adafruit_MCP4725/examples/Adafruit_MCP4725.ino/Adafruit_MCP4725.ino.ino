#include <Wire.h>
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 dacr;
Adafruit_MCP4725 dacf;

void setup(void) {
  Serial.begin(9600);
  Serial.println("Hello!");

 
  dacr.begin(0x60);
  dacf.begin(0x61);

  dacr.setVoltage(2000, true);
  dacf.setVoltage(0, true);
  }

void loop(void) {
  dacr.setVoltage(2000, false);
  dacf.setVoltage(0, false);
  //delay(100);
}
