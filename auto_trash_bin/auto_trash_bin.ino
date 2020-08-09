#include <Servo.h>
//#include <Stepper.h>



Servo myservo; 
//const int stepsPerRevolution = 200;

//Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  
  myservo.attach(9);
  int angle = 0;
  myservo.write(angle);
}

int minAngle = 0;
int maxAngle = 50;

void loop() {
  if(digitalRead(2) == LOW) {
    myservo.write(minAngle);  
    Serial.println("2 triggered");
  } else if(digitalRead(3) == LOW) {
    myservo.write(maxAngle);
    Serial.println("3 triggered");
  } else {
    Serial.println("no button");
  }
  delay(10);
  
//  int pos = 0;
  // put your main code here, to run repeatedly:

//    for (pos = minAngle; pos <= maxAngle; pos += 1) { // goes from 0 degrees to 180 degrees
//    // in steps of 1 degree
//    myservo.write(pos);              // tell servo to go to position in variable 'pos'
//    delay(15);                       // waits 15ms for the servo to reach the position
//  }
//  for (pos = maxAngle; pos >= minAngle; pos -= 1) { // goes from 180 degrees to 0 degrees
//    myservo.write(pos);              // tell servo to go to position in variable 'pos'
//    delay(15);                       // waits 15ms for the servo to reach the position
//  }

//  int motorSpeed=100;
//  myStepper.setSpeed(120);
//  myStepper.setSpeed(40);
//  myStepper.step(stepsPerRevolution / 100);
//  myStepper.step(100);
  //delay(10);
}
