/*
  Bezelie.h - Library for controlling a Bezelie.
  Created by Shigeru Kobayashi, December 4, 2015.
  Released into the public domain.

  Reference
  https://www.arduino.cc/en/Hacking/LibraryTutorial
*/
#ifndef Bezelie_h
#define Bezelie_h

#include "Arduino.h"
#include "Servo.h"

#define BEZELIE_MOTION_PITCH 0
#define BEZELIE_MOTION_ROLL  1
#define BEZELIE_MOTION_YAW   2
#define BEZELIE_MOTION_DELTA 3

#define BEZELIE_MOTION_MAX_STEPS 16
#define BEZELIE_MOTION_END_OF_SEQUENCE -1

const int defaultPitch = 90;
const int defaultRoll = 90;
const int defaultYaw = 90;

// pitch, roll, yaw, delta
const int _sequence[][BEZELIE_MOTION_MAX_STEPS][4] = {
  {
    // Say yes
    { +30,   0,   0, 100},
    { -30,   0,   0, 200},
    { +30,   0,   0, 200},
    {   0,   0,   0, 100},
    {   0,   0,   0, BEZELIE_MOTION_END_OF_SEQUENCE}
  },
  // Say no
  {
    {   0, -30,   0, 100},
    {   0, +30,   0, 200},
    {   0, -30,   0, 200},
    {   0,   0,   0, 100},
    {   0,   0,   0, BEZELIE_MOTION_END_OF_SEQUENCE},
  }
};

class Bezelie
{
  public:
    Bezelie();
    void begin(int pitchPin, int rollPin, int yawPin);
    void startMotion(int motion);
    void stopMotion();
    bool isInMotion();
    void update();

  private:
    Servo _pitchServo;
    Servo _rollServo;
    Servo _yawServo;
    int _motion;
    int _count;
    int _deltaTime;
    unsigned long _lastTime;
    bool _isInMotion;
};

#endif
