#include <Servo.h>

Servo myservo; 
Servo myservo1;
Servo myservo2;
Servo myservo3;

void setup() {
  frontDoor.attach(3);
  backDoor.attach(4);

}

void loop() {
  // put your main code here, to run repeatedly:

}

void frontDoorOpen(){
  frontDoor.write(0);
  delay(4000);

}

void frontDoorClose(){
  frontDoor.write(90);
  delay(4000);
}

void backDoorOpen(){
  backDoor.write(0);
  delay(4000);
}

void backDoorClose(){
  backDoor.write(180);
  delay(4000);
}