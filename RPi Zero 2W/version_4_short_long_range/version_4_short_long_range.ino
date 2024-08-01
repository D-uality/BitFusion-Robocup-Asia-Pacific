#include "functions.h"
#include <Arduino.h>

byte programMode = 's';
char characterBuffer[100];
unsigned long int programCount = 0;

void setup() {
  Serial.begin(115200);
  com.begin(115200);

  randomSeed(analogRead(A0));

  Serial.println("\n\nInitalising Devices");

  setupDriveServos();
  Serial.println("\tDrive servos setup ...");

  clawServo.attach(clawServoPin);
  clawServo.writeMicroseconds(2200);
  Serial.println("\tClaw servo setup ...");

  setupToF();
  Serial.println("\tTime of flight setup ...");

  setupTouch();
  Serial.println("\tTouch setup ... ");

  setupColorSensors();
  Serial.println("\tColor sensor setup ...");

  readFromEEPROM();
  Serial.println("\tCalibration setup ...");

  Serial.println("Waiting for key press ...");
  
  Serial.readString();
  while(Serial.available() == 0) { };
}

void loop() {
  if(Serial.available() > 1) programMode = Serial.read();
  Serial.print(programMode);

  // ----------------------------------------------------------------------------------------------

  if(programMode == 's') {
    Serial.print("    Stopped");
    run(0, 0);
    clawServo.writeMicroseconds(2500);
  }

  else if (programMode == 'g') running();
  else if (programMode == 't') testing();

  else if (programMode == 'c') calibration();
  else if (programMode == 'w') write();
  else if (programMode == 'r') read();

  else                         programMode = 's';

  // ----------------------------------------------------------------------------------------------

  Serial.println();
}


























