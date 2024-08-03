#include <stdint.h>
#include "functions.h"
#include <Arduino.h>

String data;
int x, y, r, greenX, redX, victimType;
int expectedCommaCount = 5;
long lastUpdate = millis();

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

  sprintf(characterBuffer, "    Victim: %d %d %d", x, y, r);
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

    run(0, 0);
    Serial.println("FINISHED!");
    Serial.readString();
    while(Serial.available() == 0) { }

  } else {
    searchAndApproach(0.5);
  }
}

void searchAndApproach(float kP) {
  unsigned long start = millis();
  unsigned long waitTime = 2000;
  unsigned long spinTime = waitTime + 750;
  unsigned long forwardTime = waitTime + 500;

  do {
    Serial.print("    Searching - LONG      ");
    comUpdate();

    if(      millis() - start < waitTime ) run(0, 0);
    else if( millis() - start < spinTime ) run(200, -200);
    else                                   start = millis();

    Serial.println();

  } while(y == -1);

  run(0, 0, 500);

  do {
    Serial.print("    Searching - LONG+SHORT");
    comUpdate();

    int error = x;
    int turn = error * kP;
    turn = constrain(turn, -50, 50);

    
    if(      millis() - start < waitTime    ) run(0, 0);
    else if( millis() - start < forwardTime ) run(150 + turn, 150 - turn);
    else                                      start = millis();

    Serial.println();

  } while(r == -1 && y != -1);

  run(0, 0, 700);

  do {
    Serial.print("    Searching - SHORT     ");
    comUpdate();

    int error = x;
    int turn = error * kP;
    turn = constrain(turn, -50, 50);

    run(120 + turn, 125 - turn);

    Serial.println();

  } while(r < 120 && r != -1 && y != -1);

  run(0, 0, 700);
}

int triangleType;
bool grabbed = false;

void grabSequence() {
  triangleType = victimType;

  run(0, 0, 500);
  clawIncrement(500, 1);

  run(150, 155, 3800);
  clawIncrement(1200, 1);

  run(-150, -155, 3000);
  run(0, 0);

  clawIncrement(1800, 2);

  grabbed = true;
}