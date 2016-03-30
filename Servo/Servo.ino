#include <Servo.h>

// Create servo object to control a servo
Servo myservo;

// Variable to store the servo position
int pos = 0;

void setup() {
  // Attaches the servo on pin 5 to the servo object
  myservo.attach(5);

  // Sets the servo position
  myservo.write(0);
  delay(1000);
  myservo.write(180);

  // Detaches the servo from the pin to free the servo and save power
  myservo.detach();
}

void loop() {
  // Nothing to do
}

