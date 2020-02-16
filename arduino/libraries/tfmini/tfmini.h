#ifndef TFMINI_H 
#define TFMINI_H

#include "Arduino.h"
#define num_lidars 4


class tfmini
{
private:
  HardwareSerial serials[num_lidars] = {Serial1, Serial2, Serial3, Serial4};
  int dists[num_lidars] = {};
  int strengths[num_lidars] = {};



public:
  tfmini();
  void getTFminiData(int* distance, int* strength, HardwareSerial &mySerial);
  void update_lidars();
  int lidar_distance(int i);
  int lidar_strength(int i);
  void print();
};

#endif
