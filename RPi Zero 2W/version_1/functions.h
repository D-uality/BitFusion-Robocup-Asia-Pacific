#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <Wire.h>
#include <Servo.h>
#include <VL53L0X.h>

extern char Buffer[100];

void setupDriveServos(int driveServoPins[4]);
void Run(int v1, int v2, int delayTime=2);

#endif