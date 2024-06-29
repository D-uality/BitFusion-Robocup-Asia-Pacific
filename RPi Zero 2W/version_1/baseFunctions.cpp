#include <stdint.h>
#include "functions.h"
#include <Arduino.h>

Servo driveServos[4];                                                                                                   // Drive servo objects
const int driveServoPins[4] = {10, 11, 9, 12};                                                                          // Drive servo pins

void setupDriveServos() {
  /* 
  Initialise, attach, and write stop for all drive servos
  */

  for(int i=0; i<4; i++) {
    driveServos[i].attach(driveServoPins[i]);
    driveServos[i].writeMicroseconds(1500);
  }
}

void Run(int v1, int v2, int delayTime=2) {
  /*
    Move the robot in a tank configuration

    Arguments:
    - v1: the velocity for the left side, mapped to 1000-2000
    - v2: the velocity for the right side, mapped to 1000-2000
    - delayTime: the delay, in milliseconds. OPTIONAL
  */

  v1 = constrain(v1, -1000, 1000);
  v2 = constrain(v2, -1000, 1000);

  driveServos[0].writeMicroseconds(1500 - v1);
  driveServos[1].writeMicroseconds(1500 - v1);
  driveServos[2].writeMicroseconds(1500 + v2);
  driveServos[3].writeMicroseconds(1500 + v2);

  if(delayTime > 0) {
    delay(delayTime);
    
    driveServos[0].writeMicroseconds(1500);
    driveServos[1].writeMicroseconds(1500);
    driveServos[2].writeMicroseconds(1500);
    driveServos[3].writeMicroseconds(1500);
  }
}

const int numberToF = 3;                                                                                                // Number of ToF sensors
Adafruit_VL53L0X ToF[numberToF] = {Adafruit_VL53L0X(), Adafruit_VL53L0X(), Adafruit_VL53L0X()};                         // ToF Sensor objects
const int xShut[numberToF]      = {14, 34, 32};                                                                         // X_SHUT pins for each ToF sensor
const int newAddress[numberToF] = {0x01, 0x02, 0x03};                                                                   // Address to be assinged for each ToF sensor
uint16_t distancesToF[3];                                                                                               // Data type to store the distances measured

void setupToF() {
  /*
  Initialise and set new address for each ToF sensors
  */

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

    ToF[i].startRangeContinuous();
    delay(10);
  }
}

Servo clawServo;                                                                                                        // Claw servo object
const int clawServoPin = 5;                                                                                             // Claw servo pin

void clawIncrement(int position, int stepTime) {
  /*
    Moves the claw to a desired position, over a period of time. 
    This ensures that the claw is not moving at the max velocity, 
    and hence dropping victims or clashing with anything.

    Arguments:
      - position: the desired PWM wavelength to use, irrelevant to the current wavelength
      - stepTimeL the time taken for each itteration whilst moving towards the desired target wavelengths
  */

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

void setupTouch() {
  /*
    Initialises each touch sensor as an input pullup, to bypass the need for a grounding resistor
  */

  for(int i=0; i<2; i++) {
    pinMode(touchPins[i], INPUT_PULLUP);
  }

}














