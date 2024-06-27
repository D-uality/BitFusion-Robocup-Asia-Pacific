#ifndef FUNCTIONS_H

  #define FUNCTIONS_H

  #include <Wire.h>
  #include <Servo.h>
  #include <Adafruit_VL53L0X.h>

  extern char characterBuffer[100];

  void setupDriveServos();
  void Run(int v1, int v2, int delayTime=2);

  extern Adafruit_VL53L0X ToF[3];
  extern VL53L0X_RangingMeasurementData_t distancesToF[3];
  void setupToF();

  extern Servo clawServo;
  extern const int clawServoPin;
  void clawIncrement(int position, int stepTime);

#endif