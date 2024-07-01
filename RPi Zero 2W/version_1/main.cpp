#include <stdint.h>
#include "functions.h"
#include <Arduino.h>


void running() {
  readColorSensors(1);
  PID(180, 3, 0, 0);
}

int error, lastError;
int integral, derivative;

void PID(int velocity, float kP, float kI, float kD) {
  error = (colourSensorValues[0] + colourSensorValues[1]) - (colourSensorValues[2] + colourSensorValues[3]);
  integral += error;
  derivative = error - lastError;

  int turn = error * kP + derivative * kD;
  run(velocity + turn, velocity - turn);

  sprintf(characterBuffer, "    %d %d  |  %d %d", error, derivative, velocity+turn, velocity-turn);
  Serial.print(characterBuffer);
  lastError = error;
}