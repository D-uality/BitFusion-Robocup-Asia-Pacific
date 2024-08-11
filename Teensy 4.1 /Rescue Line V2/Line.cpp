#include "Functions.h"

//  Note: Index 0 = Left, Index 1 = Right

//  Black & Silver values mapped
int blackMax = 10;
int blackWhite = 50;
int silverMin = 150;

bool evacExited = 0;

//  ----------------------------------------  Main Loop Function
void running() {
  readColourSensors(0);
  readTouchSensors(1);
  checkGreen();
  if(checkEvac() && !evacExited) Serial.println("Silver Detected"); //  routeEvac();
  // else if(evacExited && findFinish()) //  Search for finish line
  else if((leftBlack && leftGreen) || (rightBlack && rightGreen) || (doubleBlack[0] && doubleGreen[0]) || (doubleBlack[1] && doubleGreen[1])) detectGreen();
  else if(touchValues[0] == 0 && touchValues[1] == 0) avoidObstacle();
  else lineFollow();
}

//  ----------------------------------------  Line Following Functions
const float outerMultiplier = 3;
const float innerMultiplier = 6;
const float backMultiplier = 6;
const int forwardSpeed[2] = {270, 210};

void lineFollow() {
  readColourSensors(1);         // Read Colour sensors for line data

  //  Calculate turn
  int lineError = outerMultiplier * colourValuesMapped[0] + innerMultiplier * colourValuesMapped[1] + backMultiplier * colourValuesMapped[2] - outerMultiplier * colourValuesMapped[3] - innerMultiplier * colourValuesMapped[4] - backMultiplier * colourValuesMapped[5];
  runMotors(forwardSpeed[0] + lineError, forwardSpeed[1] - lineError, 0);
}

//  ----------------------------------------  Green Intersection Functions
int greenMin[2] = {20, 20};
int greenMax[2] = {55, 55};
bool leftBlack = 0;
bool rightBlack = 0;
bool doubleBlack[2] = {0, 0};
bool leftGreen = 0;
bool rightGreen = 0;
bool doubleGreen[2] = {0, 0};

void checkGreen() {
  //  Black Variables for checking for black and their corresponding sides
  leftBlack = colourValuesMapped[0] < blackMax && colourValuesMapped[1] < blackMax;
  rightBlack = colourValuesMapped[3] < blackMax && colourValuesMapped[4] < blackMax;
  doubleBlack[0] = leftBlack && colourValuesMapped[3] < blackMax + 5 && colourValuesMapped[4] < blackMax + 5;
  doubleBlack[1] = rightBlack && colourValuesMapped[0] < blackMax + 5 && colourValuesMapped[1] < blackMax + 5;

  //  Green Variables for checking for green and their correspoding sides
  leftGreen = colourValuesMapped[2] > greenMin[0] && colourValuesMapped[2] < greenMax[0];
  rightGreen = colourValuesMapped[5] > greenMin[1] && colourValuesMapped[5] < greenMax[1];
  doubleGreen[0] = leftGreen && colourValuesMapped[5] > greenMin[1] - 5 && colourValuesMapped[5] < greenMax[1] + 5;
  doubleGreen[1] = rightGreen && colourValuesMapped[2] > greenMin[0] - 5 && colourValuesMapped[2] < greenMax[0] + 5;
}

void detectGreen() {
  Serial.print("Green Intersections: ");
  if ((doubleBlack[0] && doubleGreen[0]) || (doubleBlack[1] && doubleGreen[1])) {
    Serial.println("Double");
    readColourSensors(1);
    runMotors(300, 300, 200);
    runMotors(350, -350, 1000);
    runUntil(blackMax, colourPins[3], 0, 300, -300, 900);
    runUntil(blackWhite, colourPins[1], 0, 250, -250, 900);
  }

  else if (leftBlack && leftGreen) {
    Serial.println("Left");
    readColourSensors(1);
    runMotors(300, 300, 300);
    runMotors(-300, 300, 200);
    runUntil(blackMax, colourPins[0], 0, -300, 300, 800);
    runUntil(blackWhite, colourPins[4], 0, -250, 250, 800);
  } 
  
  else if (rightBlack && rightGreen) {
    Serial.println("Right");
    readColourSensors(1);
    runMotors(300, 300, 300);
    runMotors(300, -300, 200);
    runUntil(blackMax, colourPins[3], 0, 300, -300, 800);
    runUntil(blackWhite, colourPins[1], 0, 250, -250, 800);
  }

  else {
    readColourSensors(1);
    Serial.println("None");
  }
}

//  ----------------------------------------  Obstacle Avoidance Functions
bool colourIsBlack[2] = {0, 0};

void circleObstacle(bool rightLaser, int turnType, bool moreThan, bool checkBlack) {
  bool laserConditionMet = 0;
  long startingTime = millis();
  int currentTime = 0;

  do {
    //  Determine movement around the obstacle
    if(turnType == 1) runMotors(300, 300, 0);
    if(turnType == 2) runMotors(300, -300, 0);
    if(turnType == 3) runMotors(-300, 300, 0);
    //  Debugging in Serial Monitor
    readColourSensors(1);
    readLaserSensors(1);
    readTouchSensors(1);

    // //  No debugging in Serial Monitor
    // readColourSensors(0);
    // readLaserSensors(0);
    // readTouchSensors(0);

    //  Inputed aser condition is met if laser sees more than 100 or less than 300
    laserConditionMet = (distance[rightLaser] > 100 && moreThan) || (distance[rightLaser] < 100 && !moreThan);

    //  Check for black
    if(checkBlack) {
      colourIsBlack[0] = (colourValuesMapped[1] < blackMax + 20 && !colourIsBlack[0]) || colourIsBlack[0];
      colourIsBlack[1] = (colourValuesMapped[4] < blackMax + 20 && !colourIsBlack[1]) || colourIsBlack[1];
    }

    //  If running into a wall, turn slightly away from the wall
    for(int touchSide = 0; touchSide < 1; touchSide++) {
      if(touchValues[touchSide] == 0 && turnType == 1) {
        if(touchSide == 0) runMotors(300, -300, 200);
        if(touchSide == 1) runMotors(-300, 300, 200);

        //  Update sensor readings after movement
        readColourSensors(0);
        readLaserSensors(0);
        readTouchSensors(0);
      }
    }

    Serial.println(laserConditionMet);
    //  Update Time
    currentTime = millis() - startingTime;
  }
  //  While laser condition not met, and time not met, or front sensors haven't seen black
  while((!laserConditionMet || currentTime < 500) && (!colourIsBlack[0] || !colourIsBlack[1]));
}

void avoidObstacle() {
  Serial.println("Obstacle Detected");
  colourIsBlack[0] = 0, colourIsBlack[1] = 0;

  //  Update Laser Values
  runMotors(0, 0, 1000);
  for(int readLasers = 0; readLasers < 100; readLasers++) readLaserSensors(0);
  readLaserSensors(1);

  //  Check for walls on left and right side of robot
  if(distance[0] > distance[1]) {
    Serial.println("Going Left");
    circleObstacle(1, 3, 0, 0);
    circleObstacle(1, 1, 1, 0);

    Serial.println("Left Obstacle Loop Phase");
    while(colourIsBlack[0] == 0 || colourIsBlack[1] == 0) {
      circleObstacle(1, 2, 1, 1);
      circleObstacle(1, 1, 1, 1);
    }

    Serial.println("Aligning to Line from Left");
    runMotors(300, 300, 200);
    runUntil(blackMax, colourPins[0], 0, -300, 300, 800);
    runUntil(blackWhite, colourPins[4], 0, -250, 250, 800);
  }
  else {
    Serial.println("Going Right");
    circleObstacle(0, 2, 0, 0);
    circleObstacle(0, 1, 1, 0);

    Serial.println("Right Obstacle Loop Phase");
    while(colourIsBlack[0] == 0 || colourIsBlack[1] == 0) {
      circleObstacle(0, 3, 1, 1);
      circleObstacle(0, 1, 1, 1);
    }

    Serial.println("Aligning to Line from Right");
    runMotors(300, 300, 200);
    runUntil(blackMax, colourPins[3], 0, 300, -300, 800);
    runUntil(blackWhite, colourPins[1], 0, 250, -250, 800);
  }
}

//  ----------------------------------------  Checking Evac Zone Function
int silverDetected = 0;

bool checkEvac() {
  if(colourValuesMapped[0] > silverMin || colourValuesMapped[1] > silverMin) silverDetected++;
  else silverDetected = 0;

  return silverDetected > 50;
}

// bool findFinish() {
//   return colourValue 
// }