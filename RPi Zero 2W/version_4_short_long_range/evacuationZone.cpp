#include <stdint.h>
#include "functions.h"
#include <Arduino.h>

String data;
int x, y, r, greenX, redX, victimType;
int expectedCommaCount = 5;
long lastUpdate = millis();

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

    run(0, 0);
    Serial.println("FINISHED!");
    Serial.readString();
    while(Serial.available() == 0) { }
  }
}

void searchAndApproach(float kP) {
  unsigned long start = millis();

  do {
    Serial.print("    Searching - LONG      ");
    comUpdate();

    run(120, -120);

    Serial.println();

  } while(y == -1);

  run(0, 0, 500);

  do {
    Serial.print("    Searching - LONG+SHORT");
    comUpdate();

    int error = x;
    int turn = error * kP;
    turn = constrain(turn, -50, 50);

    run(130 + turn, 130 - turn);

    Serial.println();

  } while(r == -1 && y != -1);

  run(0, 0, 500);

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
}

void grabSequence() {
  run(0, 0, 500);
  clawIncrement(500, 1);

  run(130, 130, 3800);
  clawIncrement(1200, 2);

  run(-150, -150, 3000);
  run(0, 0);

  clawIncrement(1800, 2);

  grabbed = true;
}

void findTriangle() {
  int triangleX;

  while(com.available() != 0) { com.read(); }
  while(com.available() == 0) { }
  comUpdate();

  do {
    Serial.print("    Locating");
    comUpdate();
    triangleX = triangleType == 0 ? greenX : redX;
    triangleType == 0 ? Serial.print("    GREEN") : Serial.print("      RED");


    run(130, -130);

    Serial.println();
  } while(triangleX == 9499);

  run(0, 0, 500);

  do {
    Serial.print("    Aligning(R)");
    comUpdate();
    triangleX = triangleType == 0 ? greenX : redX;
    triangleType == 0 ? Serial.print("    GREEN") : Serial.print("      RED");


    run(120, -120);
    
    Serial.println();
  } while(triangleX > 15);
  
  run(0, 0, 500);
  
  do {
    Serial.print("    Aligning(L)");
    comUpdate();
    triangleX = triangleType == 0 ? greenX : redX;
    triangleType == 0 ? Serial.print("    GREEN") : Serial.print("      RED");

    run(-110, 110);

    Serial.println();
  } while(triangleX < 5);
}

void dropSequence() {
  while(digitalRead(touchPins[0]) == 1 && digitalRead(touchPins[1]) == 1) {
    run(200, 210);
    Serial.println();
  }

  run(150, 150, 300);
  run(-150, -150, 500);
  run(0, 0);

  clawIncrement(1300, 1);
  delay(500);                    
  clawIncrement(2500, 1);
  run(0, 0, 1000);
  run(-150, -155, 6000);

  grabbed = false;
}