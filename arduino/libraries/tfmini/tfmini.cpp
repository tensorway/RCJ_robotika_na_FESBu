#include <tfmini.h>


tfmini::tfmini()
{
  Serial.begin(9600);
  for (int i = 0; i<num_lidars; i++)
    serials[i].begin(115200);
}



void tfmini::getTFminiData(int* distance, int* strength, HardwareSerial &mySerial) {
  static char i = 0;
  char j = 0;
  int checksum = 0; 
  static int rx[9];
  if(mySerial.available()) {  
    rx[i] = mySerial.read();
    if(rx[0] != 0x59) {
      i = 0;
    } else if(i == 1 && rx[1] != 0x59) {
      i = 0;
    } else if(i == 8) {
      for(j = 0; j < 8; j++) {
        checksum += rx[j];
      }
      if(rx[8] == (checksum % 256)) {
        *distance = rx[2] + rx[3] * 256;
        *strength = rx[4] + rx[5] * 256;
      }
      i = 0;
    } else {
      i++;
    } 
  }  
}

void tfmini::update_lidars()
{
  for (int i = 0; i<num_lidars; i++)
    getTFminiData(dists+i, strengths+i, serials[i]);
}

int tfmini::lidar_distance(int i)
{
  return dists[i];
}

int tfmini::lidar_strength(int i)
{
  return strengths[i];
}


void tfmini::print()
{
  for(int i=0; i<4; i++)
  {
  Serial.print(dists[i]);
  Serial.print(" ");
  }
  Serial.println();
}