#include <tfmini.h>

tfmini tf;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
tf.update_lidars();
tf.print();
delay(50);
}
