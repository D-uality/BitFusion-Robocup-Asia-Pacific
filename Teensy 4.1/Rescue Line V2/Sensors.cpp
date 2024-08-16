#include "Functions.h"

//  ----------------------------------------  Colour Sensor Functions
const int colourPins[6] = {A0, A1, A6, A7, A8, A9};
int colourValues[6];
int colourValuesMapped[6];
int calibratedMin[6];
int calibratedMax[6];
int prevCalibratedMin[6];
int prevCalibratedMax[6];

void readColourSensors(bool print) {
  for (byte sensorNum = 0; sensorNum < 6; sensorNum++) {
    colourValues[sensorNum] = analogRead(colourPins[sensorNum]);
    colourValuesMapped[sensorNum] = map(colourValues[sensorNum], calibratedMin[sensorNum], calibratedMax[sensorNum], 0, 100);

    if (print) {
      if(sensorNum == 0) Serial.print("C: ");
      Serial.print(colourValues[sensorNum]);
      Serial.print(",");
      Serial.print(colourValuesMapped[sensorNum]);
      Serial.print((sensorNum == 5) ? "\n" : ", ");
    }
  } 
}

void updateCalibration() {
  readColourSensors(1);         //  Raw Sensor readings 

  // Update Calibration values as car turns
  for (int sensorNum = 0; sensorNum < 6; sensorNum++) {
    calibratedMin[sensorNum] = min(calibratedMin[sensorNum], colourValues[sensorNum]);
    calibratedMax[sensorNum] = max(calibratedMax[sensorNum], colourValues[sensorNum]);
  }
}

void autoCalibration(bool spin) {
  //  Actual Calibration proccess & Storing to EEPROM
  if(spin) {
    for(int sensorNum = 0; sensorNum < 6; sensorNum++) calibratedMin[sensorNum] = 1200;
    for (int sensorNum = 0; sensorNum < 6; sensorNum++) calibratedMax[sensorNum] = 0;

    //  Spin clockwise and calibrate
    runMotors(350, -350, 0);
    for (int spinCycle = 0; spinCycle < 5000; spinCycle++) updateCalibration();

    //  Spin anti-clockwise and calibrate
    runMotors(-350, 350, 0);
    for (int spinCycle = 0; spinCycle < 5000; spinCycle++) updateCalibration();

    //  Store Min & Max data into EEPROM
    for(int sensorNum = 0; sensorNum < 6; sensorNum++) {
      if (abs(prevCalibratedMin[sensorNum] - calibratedMin[sensorNum]) > 10) {
        EEPROM.write(sensorNum, calibratedMin[sensorNum]/10);
        prevCalibratedMin[sensorNum] = calibratedMin[sensorNum];
      }
      
      if (abs(prevCalibratedMax[sensorNum] - calibratedMax[sensorNum]) > 10) {
        EEPROM.write(sensorNum+6, calibratedMax[sensorNum]/10);
        prevCalibratedMax[sensorNum] = calibratedMax[sensorNum];
      }
    }
  }

  //  Print Calibrated Min's and Max's
  for (int sensorNum = 0; sensorNum < 6; sensorNum++) {
    Serial.print(F("Ir "));
    Serial.print(sensorNum + 1);
    Serial.print(F(" = "));
    Serial.print(calibratedMin[sensorNum]);
    Serial.print(F(", "));
    Serial.print(calibratedMax[sensorNum]);
    Serial.print(sensorNum < 6 ? F(", ") : F("\n"));
  }
  mode = 's';
}

//  ----------------------------------------  Touch Sensor Functions
bool touchValues[2] = {0, 0};

void readTouchSensors(bool print) {
   //  Store Touch Readings
  touchValues[0] = digitalRead(touchPins[0]);
  touchValues[1] = digitalRead(touchPins[1]);

  //  Print Touch Values
  if(print) {
    Serial.print("T: ");
    Serial.print(touchValues[0]);
    Serial.print(", ");
    Serial.println(touchValues[1]);
  }
}

//  ----------------------------------------  Laser Sensor Functions
VL53L0X laser[3];
int previousDistance[3];
int distance[3];
uint16_t laserDistance[3];

void readLaserSensors(bool print) {
  //  Reading & Storing Laser Sensor Values without long delays (.readRangeNoBlocking)
  for(int sensorNum = 0; sensorNum < 3; sensorNum++) {
    if(laser[sensorNum].readRangeNoBlocking(laserDistance[sensorNum])) {
      previousDistance[sensorNum] = distance[sensorNum];
      distance[sensorNum] = laserDistance[sensorNum];
    }
  }
  
  // Print Laser Values
  if(print) {
    Serial.print("L: "); 
    Serial.print(distance[0]);
    Serial.print(", ");
    Serial.print(distance[1]);
    Serial.print(", ");
    Serial.println(distance[2]);
  }
}