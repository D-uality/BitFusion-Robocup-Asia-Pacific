#include <stdint.h>
#include "functions.h"
#include <Arduino.h>


void running() {
  readColorSensors();
  PID(150, 2.3, 0, 7);
}

int errorFront, errorBack, lastError;
int integral, derivative;

void PID(int velocity, float kP, float kI, float kD) {
  errorFront = (colourSensorValues[4] - colourSensorValues[5]) * 0;
  errorBack = (colourSensorValues[0] + colourSensorValues[1]) - (colourSensorValues[2] + colourSensorValues[3]);
  integral += errorFront + errorBack;
  derivative = (errorFront + errorBack) - lastError;

  int turn = (errorFront + errorBack) * kP + derivative * kD;

  sprintf(characterBuffer, "    %d %d %d  |  %d %d", errorFront, errorBack, derivative, velocity+turn, velocity-turn);
  Serial.print(characterBuffer);
  lastError = (errorFront + errorBack);

  run(velocity + turn, velocity - turn);
}