/*
  Bezelie.h - Library for controlling a Bezelie.
  Created by Shigeru Kobayashi, December 4, 2015.
  Released into the public domain.
*/

#include "Bezelie.h"

Bezelie::Bezelie()
{
  _isInMotion = false;
}

void Bezelie::begin(int pitchPin, int rollPin, int yawPin)
{
  _pitchServo.attach(pitchPin);
  _rollServo.attach(rollPin);
  _yawServo.attach(yawPin);

  _pitchServo.write(90);
  _rollServo.write(90);
  _yawServo.write(90);
}

void Bezelie::startMotion(int motion)
{
  _motion = motion;
  _count = 0;
  _lastTime = millis();
  _pitchServo.write(defaultPitch + _sequence[_motion][_count][BEZELIE_MOTION_PITCH]);
  _rollServo.write(defaultRoll + _sequence[_motion][_count][BEZELIE_MOTION_ROLL]);
  _yawServo.write(defaultYaw + _sequence[_motion][_count][BEZELIE_MOTION_YAW]);
  _deltaTime = _sequence[_motion][_count][BEZELIE_MOTION_DELTA];
  _isInMotion = true;

  Serial.println("Motion started");
}

void Bezelie::stopMotion()
{
  _pitchServo.write(90);
  _rollServo.write(90);
  _yawServo.write(90);
  _isInMotion = false;
}

bool Bezelie::isInMotion(){
  return _isInMotion;
}

void Bezelie::update()
{
  if (!_isInMotion) {
    return;
  }

  unsigned long elapsedTime = millis() - _lastTime;
  if (elapsedTime > _deltaTime) {
    _count++;
    if (_sequence[_motion][_count][BEZELIE_MOTION_DELTA] == BEZELIE_MOTION_END_OF_SEQUENCE) {
      _isInMotion = false;
      Serial.println("Motion finished");
    } else {
      _lastTime = millis();
      _pitchServo.write(defaultPitch + _sequence[_motion][_count][BEZELIE_MOTION_PITCH]);
      _rollServo.write(defaultRoll + _sequence[_motion][_count][BEZELIE_MOTION_ROLL]);
      _yawServo.write(defaultYaw + _sequence[_motion][_count][BEZELIE_MOTION_YAW]);
      _deltaTime = _sequence[_motion][_count][BEZELIE_MOTION_DELTA];
    }
  }
}

