#include <stdint.h>
#include "functions.h"
#include <Arduino.h>

void calibration() {
  Serial.print("    Calibration");

  Serial.readString();
  resetCalibrationValues();
  run(0, 0);

  while(Serial.available() == 0) { 
    calibrateColorSensors(); 
    Serial.println(); 
  }
}

void write() {
  Serial.print("    Writing to EEPROM");

  convertToBytes();
  writeToEEPROM();

  Serial.readString();
  while(Serial.available() == 0) {}

  readFromEEPROM();
  programMode = 's';
}