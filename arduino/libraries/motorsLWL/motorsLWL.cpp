#include <motorsLWL.h>
#include "Arduino.h"
#include "Arduino.h"

void motors::set_motor_pins()
{
  for (int i = 0; i < 4; i++) {
    if (motor_for_pin[i] < 0)
    {
    analogWrite(motor_pin1[i], 255);
    analogWrite(motor_pin2[i], 255 + motor_for_pin[i]);
    }
    else
    {
    analogWrite(motor_pin2[i], 255);
    analogWrite(motor_pin1[i], 255 - motor_for_pin[i]);
    }
    //Serial.print(motor_for_pin[i]);
    //Serial.print(" ");
  }
  //Serial.println();
}



void motors::set_moving_angle_beta(float angle, float speed)
{
  calc_speeds_no_upper_bound(angle, speed);
  float speed2 = speed;
  long long time = micros();
  int nes = time - last_time;
  int nes2 = millis();
  Serial.print(nes);
      Serial.print(" ");
  Serial.print(nes2);
  Serial.print(" ");

  if(time - last_time > min_microsec_to_change[0])
  {
    Serial.print(" usa ");
    for(int i=0; i<4; i++)
    {
      int delta = (time - last_time) / min_microsec_to_change[i];
      float prop_change;
      if(motor_for_pin[i] > past_motor_for_pin[i])
        prop_change = abs((motor_for_pin[i] + delta)*1./motor_for_pin[i]);
      else
        prop_change = abs((motor_for_pin[i] - delta)*1./motor_for_pin[i]);

      if(speed2 < prop_change*speed)
        speed2 = prop_change*speed;
    }
    calc_speeds_no_upper_bound(angle, speed2);
    Serial.print(speed2);
    Serial.print(" ");
    for(int i=0; i<4; i++)
      past_motor_for_pin[i] = motor_for_pin[i];
    last_time = micros();
  }
  set_motor_pins();
}

void motors::set_moving_angle(float angle, float speed)
{
  calc_speeds_no_upper_bound(angle, speed);
  long long time = micros();
  int delta = (time - last_time) / min_microsec_to_change[0];
  if (delta)
  {
    for(int i=0; i<4; i++)
    {
      if(motor_for_pin[i] > past_motor_for_pin[i] && motor_for_pin[i] > past_motor_for_pin[i] + delta)
        motor_for_pin[i] = past_motor_for_pin[i] + delta;
      else if (motor_for_pin[i] < past_motor_for_pin[i] && motor_for_pin[i] < past_motor_for_pin[i] - delta)
        motor_for_pin[i] = past_motor_for_pin[i] - delta;
      past_motor_for_pin[i] = motor_for_pin[i];
    }
    set_motor_pins();
    last_time = time;
  }
}

void motors::calc_speeds_no_upper_bound(float angle, float speed)
{
  angle_m = angle;
  speed_raw = speed;
  if(angle<0) angle+=360;
  else if(angle>360) angle-=360;
    if (angle > 180 && angle <270) 
    angle =  225 - (angle-225);

  float tanx = tan(angle*0.01745);
  if (angle > 180)
    if(angle <  270)
    {
      int cal = 255 * (tanx-1)/(tanx+1);
      motor_for_pid[0] = cal * speed;
      motor_for_pid[1] = -255 * speed;
      motor_for_pid[2] = -motor_for_pid[0];
      motor_for_pid[3] = -motor_for_pid[1];
    }
    else
    {
      int cal = 255 * (tanx+1)/(tanx-1);
      motor_for_pid[0] = -255* speed;
      motor_for_pid[1] = -cal* speed;
      motor_for_pid[2] = -motor_for_pid[0];
      motor_for_pid[3] = -motor_for_pid[1];
    }
  else
  {
    if(angle <  90)
    {
      int cal = 255 * (tanx-1)/(tanx+1);
      motor_for_pid[0] = cal* speed;
      motor_for_pid[1] = 255* speed;
      motor_for_pid[2] = -motor_for_pid[0];
      motor_for_pid[3] = -motor_for_pid[1];
    }
    else
    {
      int cal = 255 * (tanx+1)/(tanx-1);
      motor_for_pid[0] = 255* speed;
      motor_for_pid[1] = cal* speed;
      motor_for_pid[2] = -motor_for_pid[0];
      motor_for_pid[3] = -motor_for_pid[1];
    }
  }
distribute_error();
}

motors::motors()
{
  for (int i = 0; i < 4; i++)
  {
    pinMode(motor_pin1[i], OUTPUT);
    pinMode(motor_pin2[i], OUTPUT);
  }
  //delay(220);
}

void motors::distribute_error()
{
  int error_pid_cp = error_pid;
  int copy_of_motors[4] = {motor_for_pid[0], motor_for_pid[1], motor_for_pid[2], motor_for_pid[3]};
  int to_return[4] = {};
  int largest = -500;
  int index = 0;
  for (int i = 0; i < 4; i++)
  {
    for(int j = 0; j < 4; j++)
      if (copy_of_motors[j] > largest)
      {
        largest = copy_of_motors[j];
        index = j;
      }
    to_return[i] = index;
    copy_of_motors[index] = -500 ;
    largest = -500;
  }

  if (error_pid > 0){
    for(int i = 0; i <4; i++)
    {
       int x = max_motors - motor_for_pid[to_return[i]];
       if (x > error_pid_cp/(4-i))  x = error_pid_cp/(4-i);
       motor_for_pin[to_return[i]] = motor_for_pid[to_return[i]] + x;
       error_pid_cp -= x;
    } }
  else {
    for(int i = 4; i > 0;i--)
    {
       int x = min_motors - motor_for_pid[to_return[i-1]];
       if (x < error_pid_cp/(i))  x = error_pid_cp/(i);
       motor_for_pin[to_return[i-1]] = motor_for_pid[to_return[i-1]] + x;
       error_pid_cp -= x;
    }} 
}

void motors::print_motor_for_pin()
{
  Serial.print(" motor_for_pin = ");
  for(int i = 0; i<4; i++)
  {
      Serial.print(" ");
      Serial.print(motor_for_pin[i]);
  }
  Serial.println(" ");
}

void motors::pid_function()
{
  //pid_bno055.Compute();
  distribute_error();
  set_motor_pins();
}

void motors::set_error_pid(int for_pid)
{
  error_pid = for_pid;
}

void motors::rotate(int angular_speed)
{
  if(angular_speed>255)  angular_speed =  255;
  if(angular_speed<-255) angular_speed = -255;
  for (int i = 0; i < 4; i++) {
    if (angular_speed < 0)
    {
    analogWrite(motor_pin1[i], 255);
    analogWrite(motor_pin2[i], 255 + angular_speed);
    }
    else
    {
    analogWrite(motor_pin2[i], 255);
    analogWrite(motor_pin1[i], 255 - angular_speed);
    }
  }
}

void motors::speeds(float& alf, float& v)
{
  alf = angle_m;
  v = speed_raw;
}