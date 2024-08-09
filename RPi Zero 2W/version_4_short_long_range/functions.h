#ifndef FUNCTIONS_H
  #define FUNCTIONS_H

  #define com Serial1

  #include <Wire.h>
  #include <Servo.h>
  #include <Adafruit_VL53L0X.h>
  #include <EEPROM.h>

  // baseFunctions.cpp
    extern char characterBuffer[100];
    extern byte programMode;

    void setupDriveServos();
    void run(int v1, int v2, int delayTime=0);

    extern Adafruit_VL53L0X ToF[3];
    extern uint16_t distancesToF[3];
    void setupToF();
    void readToF(int sensor);

    extern Servo clawServo;
    extern const int clawServoPin;
    void clawIncrement(int position, int stepTime);

    extern const int touchPins[2];
    extern void setupTouch();

    extern const int colorSensorPins[6];
    extern void setupColorSensors();

  // colorSensor.cpp
    extern int colourSensorValues[6];
    extern void resetCalibrationValues();
    extern void calibrateColorSensors();
    extern void convertToBytes();
    extern void writeToEEPROM();
    extern void readFromEEPROM();
    extern void readColorSensors(int mode=0);

  // calibration.cpp
    extern void calibration();
    extern void write();

  // running.cpp
    extern void running();

  // evacuationZone.cpp
    extern String data;
    extern int x, y, r;
    
    extern void comUpdate();
    extern void splitData();

    extern void evacuationZone();
    extern void searchAndApproach(float kP);
    extern void grabSequence();
    extern void findTriangle();
    extern void dropSequence();

  // testing.cpp
    extern void testing();

  // read.cpp
    extern void read();

  
#endif