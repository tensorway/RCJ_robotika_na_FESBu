#ifndef MOTORS_H
#define MOTORS_H


#define max_motors 255 
#define min_motors (-255)


class motors
{
private:
  //const int motor_pin2[4] = {6, 4, 3, 5};
  //const int motor_pin1[4] = {20, 22, 23, 21};
  const int motor_pin2[4] = {3, 4, 8, 10};
  const int motor_pin1[4] = {2, 5, 6, 29}; 
  const int min_microsec_to_change[4] = {10, 10, 10, 10};
  long long last_time = 0;
  int past_motor_for_pin[4] = {0, 0, 0, 0};
  int motor_for_pid[4] = {0, 0, 0, 0};
  int motor_for_pin[4] = {100, 100, 100, 100};
  float angle_m = 0;
  float speed_raw = 0;
  volatile int error_pid = 0.0;
  void set_motor_pins();
  void distribute_error();



public:
  void set_moving_angle(float angle, float speed);
  void set_moving_angle_beta(float angle, float speed);
  void calc_speeds_no_upper_bound(float angle, float speed);
  void print_motor_for_pin();
  void pid_function();
  void rotate(int angular_speed);
  void set_error_pid(int for_pid);
  motors();
  void speeds(float &alf, float& v);

};

#endif
