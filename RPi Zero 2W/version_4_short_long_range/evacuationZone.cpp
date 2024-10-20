#include <stdint.h>
#include "functions.h"
#include <Arduino.h>

String data;
int x, y, r, greenX, redX, victimType;
int greenBuffer[5] = {9499, 9499, 9499, 9499, 9499};
int redBuffer[5]   = {9499, 9499, 9499, 9499, 9499};
int expectedCommaCount = 5;
long lastUpdate = millis();
unsigned long counter = 0;

int triangleType;
bool grabbed = false;

void comUpdate() {
  if(com.available() && millis() - lastUpdate > 7) {
    data = "";
    while(com.available() > 0) {
      char character = com.read();
      data += character;
    }

    int commaCount = 0;
    for(int i=0; i<data.length(); i++) { if(data.charAt(i) == ',') commaCount ++; }
    if(commaCount == expectedCommaCount) splitData();

    lastUpdate = millis();
  }

  sprintf(characterBuffer, "    Victim: %d %d %d    Green: %d    Red: %d    victimType: %d", x, y, r, greenX, redX, victimType);
  Serial.print(characterBuffer);
}

void splitData() {
  int commaPositions[expectedCommaCount];
  int commaOccurance = 0;

  for(int i=0; i<data.length(); i++) {
    if(data.charAt(i) == ',') {
      commaPositions[commaOccurance] = i;
      commaOccurance += 1;
    }
  }

  x            = data.substring(0                    , commaPositions[0]).toInt();
  y            = data.substring(commaPositions[0] + 1, commaPositions[1]).toInt();
  r            = data.substring(commaPositions[1] + 1, commaPositions[2]).toInt();
  greenX       = data.substring(commaPositions[2] + 1, commaPositions[3]).toInt();
  redX         = data.substring(commaPositions[3] + 1, commaPositions[4]).toInt();
  victimType   = data.substring(commaPositions[4] + 1, data.length() + 1).toInt();

  greenBuffer[counter % 5] = greenX;
  redBuffer[  counter % 5] = redX;
  counter ++;
}

void evacuationZone() {
  Serial.print("    Evacuation");
  comUpdate();

  if(r > 100) {
    grabSequence();
  } else {
    searchAndApproach(0.5);
  }

  if(grabbed == true) {
    findTriangle();
    dropSequence();
  }
}

void searchAndApproach(float kP) {
  unsigned long start = millis();
  unsigned long waitTime = 500;
  unsigned long forwardTime = waitTime + 500;

  do {
    Serial.print("    Searching - LONG      ");
    comUpdate();

    run(130, -130);

    Serial.println();

  } while(y == -1);

  run(0, 0, 500);
  Serial.println();

  do {
    Serial.print("    Searching - LONG+SHORT");
    comUpdate();

    int error = x;
    int turn = error * kP;
    turn = constrain(turn, -50, 50);

    if(      millis() - start < waitTime    ) run(0, 0);
    else if( millis() - start < forwardTime ) run(130 + turn, 130 - turn);
    else                                      start = millis();

    Serial.println();

  } while(r == -1 && y != -1);

  run(0, 0, 500);
  Serial.println();

  do {
    Serial.print("    Searching - SHORT     ");
    comUpdate();

    int error = x;
    int turn = error * kP;
    turn = constrain(turn, -50, 50);

    run(130 + turn, 130 - turn);

    Serial.println();

  } while(r < 120 && r != -1 && y != -1);

  triangleType = victimType;

  run(0, 0, 500);
  Serial.println();
}

void grabSequence() {
  run(0, 0, 500);
  clawIncrement(500, 1);

  run(170, 170, 3800);
  clawIncrement(1200, 2);


  run(-150, -150, 3000);
  run(0, 0);
  clawIncrement(1800, 2);

  do {
    Serial.print("Moving back (F)");
    run(-150, -150);
    readToF(0);
    Serial.println();
  } while(distancesToF[0] < 350);
  
  do {
    Serial.print("Moving back (L)");
    run(-150, -110);
    readToF(1);
    Serial.println();
  } while(distancesToF[1] < 350);
  
  do {
    Serial.print("Moving back (R)");
    run(-110, -150);
    readToF(2);
    Serial.println();
  } while(distancesToF[2] < 350);

  run(0, 0);

  grabbed = true;
}

void findTriangle() {
  while(com.available() != 0) { com.read(); }
  while(com.available() == 0) { }
  comUpdate();

  int triangleX = 0;
  long triangleTotal = 0, triangleAverage = 0;

  do {
    Serial.print("    Locating");
    
    triangleType == 0 ? Serial.print("    GREEN") : Serial.print("    RED");
    comUpdate();
    for(int i=0; i<5; i++) { triangleTotal += triangleType == 0 ? greenBuffer[i] : redBuffer[i]; }
    triangleAverage = triangleTotal / 5;

    run(120, -120);

    sprintf(characterBuffer, "    triangle (total, average): %ld %ld", triangleTotal, triangleAverage);
    Serial.print(characterBuffer);

    triangleTotal = 0;
    Serial.println();
  } while(triangleAverage > 2000);

  run(0, 0);
  while(com.available() != 0) { com.read(); }
  while(com.available() == 0) { }
  comUpdate();

  do {
    Serial.print("    Aligning(LR)");

    triangleType == 0 ? Serial.print("    GREEN") : Serial.print("    RED");
    comUpdate();
    triangleX = triangleType == 0 ? greenX : redX;

    run(115, -115);

    Serial.println();
  } while(triangleX > 0);

  run(0, 0);
  while(com.available() != 0) { com.read(); }
  while(com.available() == 0) { }
  comUpdate();
  
  do {
    Serial.print("    Aligning(LL)");

    triangleType == 0 ? Serial.print("    GREEN") : Serial.print("    RED");
    comUpdate();
    triangleX = triangleType == 0 ? greenX : redX;

    run(-110, 110);

    Serial.println();
  } while(triangleX < 0);

  do {
    Serial.print("Moving Closer");
    run(200, 200);
    readToF(0);
    Serial.println();
  } while(distancesToF[0] > 320);
  
  do {
    Serial.print("Moving Further");
    run(-200, -200);
    readToF(0);
    Serial.println();
  } while(distancesToF[0] < 320);

  while(com.available() != 0) { com.read(); }
  while(com.available() == 0) { }
  comUpdate();

  do {
    Serial.print("    Aligning(SR)");

    triangleType == 0 ? Serial.print("    GREEN") : Serial.print("    RED");
    comUpdate();
    triangleX = triangleType == 0 ? greenX : redX;

    run(115, -115);

    Serial.println();
  } while(triangleX > 0);

  run(0, 0);
  while(com.available() != 0) { com.read(); }
  while(com.available() == 0) { }
  comUpdate();
  
  unsigned long start = millis();
  unsigned long turnTime = 200;
  unsigned long waitTime = turnTime + 500;

  do {
    Serial.print("    Aligning(SL)");

    triangleType == 0 ? Serial.print("    GREEN") : Serial.print("    RED");
    comUpdate();
    triangleX = triangleType == 0 ? greenX : redX;

    if(millis() - start < 200)            run(-100, 100);
    else if (millis() - start < waitTime) run(0, 0);
    else                                  start = millis();

    Serial.println();
  } while(triangleX < 0);

  run(0, 0);
}

void dropSequence() {
  while(digitalRead(touchPins[0]) == 1 && digitalRead(touchPins[1]) == 1) {
    run(250, 250);
    Serial.println();
  }

  run(150, 150, 300);
  run(-150, -150, 500);
  run(0, 0);

  clawIncrement(1300, 1);
  delay(500);
  clawIncrement(2500, 1);

  run(0, 0, 1000);
  run(-150, -150, 6000);

  grabbed = false;
}