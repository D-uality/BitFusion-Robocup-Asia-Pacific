#include <stdint.h>
#include "functions.h"
#include <Arduino.h>


void running() {
  readColorSensors();
  PID(180, 5, 0, 0);
}

int errorFront, errorBack, error, lastError;
int integral, derivative;

void PID(int velocity, float kP, float kI, float kD) {
  // errorFront = (colourSensorValues[4] - colourSensorValues[5]) * 0;
  // errorBack = (colourSensorValues[0] + colourSensorValues[1]) - (colourSensorValues[2] + colourSensorValues[3]);
  
  // error = errorFront * 0 + errorBack;
  error = (colourSensorValues[0] + colourSensorValues[1]) - (colourSensorValues[2] + colourSensorValues[3]);
  integral += error;
  derivative = error - lastError;

  int turn = error * kP + derivative * kD;

  sprintf(characterBuffer, "    %d %d %d | %d  |  %d %d", errorFront, errorBack, error, derivative, velocity+turn, velocity-turn);
  Serial.print(characterBuffer);
  lastError = error;

  run(velocity + turn, velocity - turn);
}