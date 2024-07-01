#include <stdint.h>
#include "functions.h"
#include <Arduino.h>

int colourSensorValues[6];
int minimumColourSensorValues[6] = {1023, 1023, 1023, 1023, 1023, 1023};
int maximumColourSensorValues[6] = {0,    0,    0,    0,    0,    0};

void resetCalibrationValues() {
  for(int i=0; i<6; i++) {
    minimumColourSensorValues[i] = 1023;
    maximumColourSensorValues[i] = 0;
  }
}
void calibrateColorSensors() {

  for(int i=0; i<6; i++) {
    colourSensorValues[i] = analogRead(colorSensorPins[i]);

    minimumColourSensorValues[i] = min(minimumColourSensorValues[i], colourSensorValues[i]);
    maximumColourSensorValues[i] = max(maximumColourSensorValues[i], colourSensorValues[i]);
  }

  sprintf(characterBuffer, "    Minimum: %d %d %d %d %d %d    |    Maximum %d %d %d %d %d %d", minimumColourSensorValues[0], minimumColourSensorValues[1], minimumColourSensorValues[2], minimumColourSensorValues[3], minimumColourSensorValues[4], minimumColourSensorValues[5], maximumColourSensorValues[0], maximumColourSensorValues[1], maximumColourSensorValues[2], maximumColourSensorValues[3], maximumColourSensorValues[4], maximumColourSensorValues[5]);
  Serial.print(characterBuffer);
}

uint8_t minimumHighByte[6];
uint8_t maximumHighByte[6];
uint8_t minimumLowByte[6];
uint8_t maximumLowByte[6];

void convertToBytes() {
  for(int i=0; i<6; i++) {
    minimumHighByte[i] = (minimumColourSensorValues[i] >> 8) & 0xFF; 
    minimumLowByte[i]  = minimumColourSensorValues[i]        & 0xFF;
    maximumHighByte[i] = (maximumColourSensorValues[i] >> 8) & 0xFF;
    maximumLowByte[i]  = maximumColourSensorValues[i]        & 0xFF;
  }

  sprintf(characterBuffer, "    Minimum: %d-%d %d-%d %d-%d %d-%d %d-%d %d-%d", minimumHighByte[0], minimumLowByte[0], minimumHighByte[1], minimumLowByte[1], minimumHighByte[2], minimumLowByte[2], minimumHighByte[3], minimumLowByte[3], minimumHighByte[4], minimumLowByte[4], minimumHighByte[5], minimumLowByte[5]);
  Serial.print(characterBuffer);
  sprintf(characterBuffer, "    Maximum: %d-%d %d-%d %d-%d %d-%d %d-%d %d-%d", maximumHighByte[0], maximumLowByte[0], maximumHighByte[1], maximumLowByte[1], maximumHighByte[2], maximumLowByte[2], maximumHighByte[3], maximumLowByte[3], maximumHighByte[4], maximumLowByte[4], maximumHighByte[5], maximumLowByte[5]);
  Serial.print(characterBuffer);
}

void writeToEEPROM() {
  uint8_t byteList[24];

  for(int i=0; i<6; i++) {
    byteList[i*4    ] = minimumHighByte[i];
    byteList[i*4 + 1] = minimumLowByte[i];
    byteList[i*4 + 2] = maximumHighByte[i];
    byteList[i*4 + 3] = maximumLowByte[i];
  }

  for(int i=0; i<24; i++) {
    sprintf(characterBuffer, "        EEPROM.write(%d, %d)", i, byteList[i]);
    Serial.println();
    Serial.print(characterBuffer);

    EEPROM.write(i, byteList[i]);
  }
}

void readFromEEPROM() {
  for(int i=0; i<6; i++) {
    minimumColourSensorValues[i] = EEPROM.read(i*4    ) * 256 + EEPROM.read(i*4 + 1);
    maximumColourSensorValues[i] = EEPROM.read(i*4 + 2) * 256 + EEPROM.read(i*4 + 3);
  }

  sprintf(characterBuffer, "    Minimum: %d %d %d %d %d %d", minimumColourSensorValues[0], minimumColourSensorValues[1], minimumColourSensorValues[2], minimumColourSensorValues[3], minimumColourSensorValues[4], minimumColourSensorValues[5]);
  Serial.print(characterBuffer);
  sprintf(characterBuffer, "    Maximum: %d %d %d %d %d %d", maximumColourSensorValues[0], maximumColourSensorValues[1], maximumColourSensorValues[2], maximumColourSensorValues[3], maximumColourSensorValues[4], maximumColourSensorValues[5]);
  Serial.print(characterBuffer);
}

void readColorSensors(int mode) {
  for(int i=0; i<6; i++) {
    colourSensorValues[i] = analogRead(colorSensorPins[i]);

    if     (mode == 0) colourSensorValues[i] = map(colourSensorValues[i], minimumColourSensorValues[i], maximumColourSensorValues[i], 0, 100);
    else if(mode == 1) colourSensorValues[i] = map(colourSensorValues[i], minimumColourSensorValues[i], maximumColourSensorValues[i], 0, 500);
    else if(mode == 2) colourSensorValues[i] = map(colourSensorValues[i], minimumColourSensorValues[i], maximumColourSensorValues[i], 0, 1023);
  }

  sprintf(characterBuffer, "    Color Sensor Values: %d %d %d %d %d %d", colourSensorValues[0], colourSensorValues[1], colourSensorValues[2], colourSensorValues[3], colourSensorValues[4], colourSensorValues[5]);
  Serial.print(characterBuffer);
}







