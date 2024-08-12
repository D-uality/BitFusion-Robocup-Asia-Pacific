#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <MPU9250.h>

// Line Asia V1.ino
extern int mode;
extern int touchPins[2];
extern int xShutPins[3];

// Line.cpp
void running();
void lineFollow();
extern bool leftBlack;
extern bool rightBlack;
extern bool doubleBlack[2];
extern bool leftGreen;
extern bool rightGreen;
extern bool doubleGreen[2];
void checkGreen();
void detectGreen();
void avoidObstacle();
extern bool evacExited;
bool checkEvac();

// Sensors.cpp
extern const int colourPins[6];
extern int colourValues[6];
extern int colourValuesMapped[6];
extern int calibratedMin[6];
extern int calibratedMax[6];
extern int prevCalibratedMin[6];
extern int prevCalibratedMax[6];
void readColourSensors(bool print);
void autoCalibration(bool spin);
extern bool touchValues[2];
void readTouchSensors(bool print);
extern VL53L0X laser[3];
extern int previousDistance[3];
extern int distance[3];
void readLaserSensors(bool print);

// Servos.cpp
extern const int frontLeftPin;
extern const int frontRightPin;
extern const int backLeftPin;
extern const int backRightPin;
extern const int liftClawPin;
extern const int grabClawPin;
extern const int frequency;
extern const int period;
extern const int clawUp;
extern const int clawGrab;  
void runMotors(int leftSpeedMicroseconds, int rightSpeedMicroseconds, int duration);
void runUntil(int targetValue, int sensorPin, bool moreThan, int leftSpeed, int rightSpeed, int maxTime);

// test.cpp
void test();

#endif