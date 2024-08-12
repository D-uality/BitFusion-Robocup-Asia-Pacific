#include "Functions.h"

const int frontLeftPin = 2;                 // Front left servo pin
const int frontRightPin = 3;                // Front right servo pin
const int backLeftPin = 4;                  // Back left servo pin
const int backRightPin = 5;                 // Back right servo pin
const int liftClawPin = 7;                  // Lift Claw servo pin
const int grabClawPin = 8;                  // Grab Claw servo pin
const int frequency = 330;                  // 330Hz for servo control

const int stop = 128;                       // Half of Maximum PWM of 256
const int maxMotorFrequency = 80;           // Maximum value for forward rotation
const int minMotorFrequency = -80;          // Minimum value for backward rotation

const int clawUp = 60;                  // Lift servo fully up value
const int clawGrab = 120;               // Lift servo grab value 120

void runMotors(int leftSpeedMicroseconds, int rightSpeedMicroseconds, int duration) {
  // Map 1000-2000 microseconds to -90 to 90 and constrain the values
  int leftMotorSpeed = constrain(map(leftSpeedMicroseconds, -1000, 1000, minMotorFrequency, maxMotorFrequency), minMotorFrequency, maxMotorFrequency);
  int rightMotorSpeed = constrain(map(rightSpeedMicroseconds, -1000, 1000, minMotorFrequency, maxMotorFrequency), minMotorFrequency, maxMotorFrequency);

  // Write the calculated speed to the servos
  analogWrite(frontLeftPin, stop + leftMotorSpeed);
  analogWrite(backLeftPin, stop + leftMotorSpeed);
  analogWrite(frontRightPin, stop - rightMotorSpeed);
  analogWrite(backRightPin, stop - rightMotorSpeed);

  delay(duration);
}

void runUntil(int targetValue, int sensorPin, bool moreThan, int leftSpeed, int rightSpeed, int maxTime) {
  bool sensorConditionMet = 0;
  long startTime = millis();
  int currentTime = 0;

  do {
    for(int colourSensors = 0; colourSensors < 6; colourSensors++) {
      if(sensorPin == colourPins[colourSensors]) {
        // Serial.println(colourSensors);
        readColourSensors(0);
        // Serial.println(colourValuesMapped[colourSensors]);
        sensorConditionMet = (moreThan && colourValuesMapped[colourSensors] > targetValue) || (!moreThan && colourValuesMapped[colourSensors] < targetValue);
        break;
      }
    }
    for(int laserSensors = 0; laserSensors < 3; laserSensors++) {
      if(sensorPin == xShutPins[laserSensors]) {
        // Serial.println("Laser");
        readLaserSensors(0);
        sensorConditionMet = (moreThan && distance[laserSensors] > targetValue) || (!moreThan && distance[laserSensors] < targetValue);
        break;
      }
    }
    for(int touchSensors = 0; touchSensors < 2; touchSensors++) {
      if(sensorPin == touchPins[touchSensors]) {
        // Serial.println("Touch");
        readTouchSensors(0);
        sensorConditionMet = (moreThan && touchValues[touchSensors] > targetValue) || (!moreThan && touchValues[touchSensors] < targetValue);
        break;
      }
    }
    // Serial.print("Current Time: ");
    // Serial.print(currentTime);
    // Serial.print(", Sensor Condition ");
    // if(sensorConditionMet) Serial.println("Met");
    // if(!sensorConditionMet) Serial.println("not Met");
    runMotors(leftSpeed, rightSpeed, 0);
    currentTime = millis() - startTime;
  } while(!sensorConditionMet && currentTime < maxTime);
} 