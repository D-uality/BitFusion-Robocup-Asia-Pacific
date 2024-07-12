#ifndef FUNCTIONS_H
  #define FUNCTIONS_H
  #define com Serial1

  #include <Wire.h>
  #include <Servo.h>
  #include <Adafruit_VL53L0X.h>
  #include <EEPROM.h>

  // baseFunctions.cpp
    extern char characterBuffer[100];

    void setupDriveServos();
    void run(int v1, int v2, int delayTime=0);

    extern Adafruit_VL53L0X ToF[3];
    extern uint16_t distancesToF[3];
    void setupToF();

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

  // running.cpp
    extern void running();
    extern void readColorSensors(int mode=0);
    void PID(int velocity, float kP, float kI, float kD);

  // evacuationZone.cpp
    extern String data;
    extern int x, y, r, greenX, greenY, redX, redY, victimType;
    extern bool grabbed;
    extern int triangleType;
    extern int victimsFound;

    extern void comUpdate();
    extern void splitData();
    extern void findVictim();
    extern void findWhileTurning();
    extern void approachVictim(float kP);
    extern void grabSequence();
    extern void evacuation();
    extern void findTriangle();
    

    extern bool grabbed;

#endif