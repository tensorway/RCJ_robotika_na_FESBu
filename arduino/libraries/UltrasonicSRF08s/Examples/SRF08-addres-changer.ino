#include <Wire.h>

#define oldad 0x72
#define newad 0xEE

/*
   0x70 <=> 0xE0
   0x71 <=> 0xE2
   0x72 <=> 0xE4
   0x73 <=> 0xE6
   0x74 <=> 0xE8
   0x75 <=> 0xEA
   0x76 <=> 0xEC
   0x77 <=> 0xEE
 */

void setup() {

  Wire.begin();
  
  delay(100);

    Wire.beginTransmission(oldad);
    Wire.write(0x00);
    Wire.write(0xA0);
    Wire.endTransmission();
    delay(60);
    
    Wire.beginTransmission(oldad);
    Wire.write(0x00);
    Wire.write(0xAA);
    Wire.endTransmission();
    delay(60);
    
    Wire.beginTransmission(oldad);
    Wire.write(0x00);
    Wire.write(0xA5);
    Wire.endTransmission();
    delay(60);
    
    Wire.beginTransmission(oldad);
    Wire.write(0x00);
    Wire.write(newad);
    Wire.endTransmission();

}
  
void loop(){
  pinMode(13, OUTPUT);
  digitalWrite(13, 1);
}
