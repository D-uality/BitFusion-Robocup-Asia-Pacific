#include "functions.h"
#include <Arduino.h>

Servo driveServos[4];
// int driveServoPins[4] = {10, 11, 9, 12};

void setupDriveServos(int driveServoPins[4]) {
  /* 
  Initalise, attach, and write stop for all drive servos
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