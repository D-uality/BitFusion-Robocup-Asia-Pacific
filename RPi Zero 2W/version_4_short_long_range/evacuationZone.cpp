#include <stdint.h>
#include "functions.h"
#include <Arduino.h>

String data;
int x, y, r;
int expectedCommaCount = 2;
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
}

void evacuationZone() {
  Serial.print("    Evacuation");
  x = y = r = 0;

  searchAndApproach(1.5);

  run(0, 0);
  Serial.println("FINISHED!");
  Serial.readString();
  while(Serial.available() == 0) { }
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

  } while(y <= 0);

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

  } while(r == 0);

  do {
    Serial.print("    Searching - SHORT     ");
    comUpdate();

    int error = x;
    int turn = error * kP;
    turn = constrain(turn, -25, 25);

    run(120 + turn, 120 - turn);

    Serial.println();
    
  } while(r < 120);
}