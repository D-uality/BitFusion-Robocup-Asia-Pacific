#include <stdint.h>
#include "functions.h"
#include <Arduino.h>

Servo driveServos[4];                                                                                                   // Drive servo objects
const int driveServoPins[4] = {10, 4, 9, 12};                                                                          // Drive servo pins

/* Initialise, attach, and write stop for all drive servos */
void setupDriveServos() {
  for(int i=0; i<4; i++) {
    driveServos[i].attach(driveServoPins[i]);
    driveServos[i].writeMicroseconds(1500);
  }
}

/*
  Move the robot in a tank configuration

  Arguments:
  - v1: the velocity for the left side, mapped to 1000-2000
  - v2: the velocity for the right side, mapped to 1000-2000
  - delayTime: the delay, in milliseconds. OPTIONAL
*/
void run(int v1, int v2, int delayTime) {
  int calculatedPWM[4] = {0, 0, 0, 0};

  float gradientNegative[4]    = {0.60, 0.56, 0.58, 0.61};
  float yInterceptsNegative[4] = {-88.48, -66.02, -74.74, -70.33};
  float gradientPositive[4]    = {0.59, 0.57, 0.55, 0.57};
  float yInterceptsPositive[4] = {77.72, 42.17, 73.16, 62.34};
  
  for(int i=0; i<2; i++) {
    if      (v1 < yInterceptsNegative[i]) calculatedPWM[i] = gradientNegative[i] * v1 + yInterceptsNegative[i];
    else if (v1 > yInterceptsPositive[i]) calculatedPWM[i] = gradientPositive[i] * v1 + yInterceptsPositive[i];
    else                                  calculatedPWM[i] = 0;
    
    calculatedPWM[i] = constrain(calculatedPWM[i], -500, 500);
  }

  v2 *= -1;
  for(int i=2; i<4; i++) {
    if      (v2 < yInterceptsNegative[i]) calculatedPWM[i] = gradientNegative[i] * v2 + yInterceptsNegative[i];
    else if (v2 > yInterceptsPositive[i]) calculatedPWM[i] = gradientPositive[i] * v2 + yInterceptsPositive[i];
    else                                  calculatedPWM[i] = 0;
    
    calculatedPWM[i] = constrain(calculatedPWM[i], -500, 500);
  }

  driveServos[0].writeMicroseconds(1500 + calculatedPWM[0]);
  driveServos[1].writeMicroseconds(1500 + calculatedPWM[1]);
  driveServos[2].writeMicroseconds(1500 + calculatedPWM[2]);
  driveServos[3].writeMicroseconds(1500 + calculatedPWM[3]);

  delay(delayTime);
}

const int numberToF = 3;                                                                                                // Number of ToF sensors
Adafruit_VL53L0X ToF[numberToF] = {Adafruit_VL53L0X(), Adafruit_VL53L0X(), Adafruit_VL53L0X()};                         // ToF Sensor objects
const int xShut[numberToF]      = {14, 32, 34};                                                                         // X_SHUT pins for each ToF sensor
const int newAddress[numberToF] = {0x08, 0x09, 0x0a};                                                                   // Address to be assinged for each ToF sensor
uint16_t distancesToF[3];                                                                                               // Data type to store the distances measured

/* Initialise and set new address for each ToF sensors */
void setupToF() {
  // Turn off all sensors
  for(int i=0; i<numberToF; i++) {
    pinMode(xShut[i], OUTPUT);
    digitalWrite(xShut[i], LOW);
  }
  
  delay(10);

  // Setup each sensor, one by one
  for(int i=0; i<numberToF; i++) {
    digitalWrite(xShut[i], HIGH);
    delay(10);

    if( !ToF[i].begin(newAddress[i]) ) {
      sprintf(characterBuffer, "Failed to initialise ToF[%d]. Waiting for manual restart!", i);
      Serial.println(characterBuffer);
      
      while(true) {}
    }

    delay(10);
  }
}

void readToF(int sensor) {
  VL53L0X_RangingMeasurementData_t measure;
  
  ToF[sensor].rangingTest(&measure, false);
  
  if(measure.RangeStatus != 4) {
    distancesToF[sensor] = measure.RangeMilliMeter;
    sprintf(characterBuffer, "    ToF[%d]: %d", sensor, distancesToF[sensor]);
    Serial.print(characterBuffer);
  }
}

Servo clawServo;                                                                                                        // Claw servo object
const int clawServoPin = 5;                                                                                             // Claw servo pin

/*
  Moves the claw to a desired position, over a period of time. 
  This ensures that the claw is not moving at the max velocity, 
  and hence dropping victims or clashing with anything.

  Arguments:
    - position: the desired PWM wavelength to use, irrelevant to the current wavelength
    - stepTimeL the time taken for each itteration whilst moving towards the desired target wavelengths
*/
void clawIncrement(int position, int stepTime) {

  int currentPosition = clawServo.readMicroseconds();

  if(currentPosition > position) {
    for(int i=currentPosition; i>position; i--) {
      clawServo.writeMicroseconds(i);
      delay(stepTime);
    }
  }
  
  else {
    for(int i=currentPosition; i<position; i++) {
      clawServo.writeMicroseconds(i);
      delay(stepTime);
    }
  }
}

const int touchPins[2] = {2, 3};

/* Initialises each touch sensor as an input pullup, to bypass the need for a grounding resistor */
void setupTouch() {
  for(int i=0; i<2; i++) { pinMode(touchPins[i], INPUT_PULLUP); }
}

const int colorSensorPins[6] = {A3, A5, A7, A9, A11, A13};                                                              // IR Sensor Pins

/* Change all pins assoicated with color sensors to being analog inputs */
void setupColorSensors() {
  for(int i=0; i<6; i++) {
    pinMode(colorSensorPins[i], INPUT);
  }
}