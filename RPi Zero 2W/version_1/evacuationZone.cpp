#include <stdint.h>
#include "functions.h"
#include <Arduino.h>

String data;
int x, y, r, greenX, greenY, redX, redY, victimType;
long lastUpdate = millis();

void comUpdate() {
  if(com.available() && millis() - lastUpdate > 5) {
    data = "";
    while(com.available() > 0) {
      char character = com.read();
      data += character;
    }

    int commaCount = 0;
    for(int i=0; i<data.length(); i++) { if(data.charAt(i) == ',') commaCount ++; }
    if(commaCount == 7) splitData();

    lastUpdate = millis();
  }
}

void splitData() {
  int commaPositions[7];
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
  greenY       = data.substring(commaPositions[3] + 1, commaPositions[4]).toInt();
  redX         = data.substring(commaPositions[4] + 1, commaPositions[5]).toInt();
  redY         = data.substring(commaPositions[5] + 1, commaPositions[6]).toInt();
  victimType   = data.substring(commaPositions[6] + 1, data.length() + 1).toInt();

  sprintf(characterBuffer, "    x: %d y: %d r: %d Green: %d %d Red: %d %d victimType: %d", x, y, r, greenX, greenY, redX, redY, victimType);
  Serial.print(characterBuffer);
}

void findVictim() {
  long startSearchTime = millis();
  while(r == 0 || (victimType == 1 && victimsFound < 2) || (victimType == 0 && victimsFound == 2)) {
    Serial.print("Searching");

    comUpdate();
    if(Serial.available() > 0) { break; Serial.readString(); }
    run(153, 150);

    sprintf(characterBuffer, "    x: %d y: %d r: %d Green: %d %d Red: %d %d victimType: %d", x, y, r, greenX, greenY, redX, redY, victimType);
    Serial.print(characterBuffer);

    if(digitalRead(touchPins[0]) == 0 || digitalRead(touchPins[1]) == 0 || millis() - startSearchTime > 16000) { startSearchTime = millis(); findWhileTurning(); }

    Serial.println();
  }
}

void findWhileTurning() {
  run(-160, -150, 1200);

  long startRotationTime = millis();
  int turnTime = random(1200, 2700);

  while(millis() - startRotationTime < turnTime && r == 0) {
    Serial.print("Search(Turn)");
    
    comUpdate();
    if(Serial.available() > 0) { break; Serial.readString(); }

    sprintf(characterBuffer, "    x: %d y: %d r: %d Green: %d %d Red: %d %d victimType: %d", x, y, r, greenX, greenY, redX, redY, victimType);
    Serial.print(characterBuffer);

    run(150, -150);

    Serial.println();
  }

  run(0, 0, 500);
}

void approachVictim(float kP) {
  while(r > 70 && r != 0) {
    run(-120, -120);
    comUpdate();
  }

  while(r < 90 && r != 0) {
    Serial.print("Appraching");

    comUpdate();
    if(Serial.available() > 0) { break; Serial.readString(); }

    int error = x - 240;
    int turn = error * kP;
    turn = constrain(turn, -30, 30);

    sprintf(characterBuffer, "    %d | %d %d    ", error, 150 + turn, 150 - turn);
    Serial.print(characterBuffer);
    sprintf(characterBuffer, "    x: %d y: %d r: %d Green: %d %d Red: %d %d victimType: %d", x, y, r, greenX, greenY, redX, redY, victimType);
    Serial.print(characterBuffer);

    run(120 + turn, 120 - turn);

    Serial.println();
  }
}

int triangleType;
int victimsFound = 0;

void grabSequence() {
  if(r > 85) {
    triangleType = victimType;

    run(0, 0, 500);
    clawIncrement(500, 1);

    run(120, 120, 3800);
    clawIncrement(1200, 1);

    run(-120, -120, 3000);
    run(0, 0);

    clawIncrement(1800, 2);

    grabbed = true;
  }
}

void findTriangle() {
  int triangleX;

  if(grabbed == true) {
    delay(3000);

    while(!ToF[0].isRangeComplete()) {}
    do {
      Serial.print("Backing");

      if(ToF[0].isRangeComplete()) distancesToF[0] = ToF[0].readRange();

      sprintf(characterBuffer, "Front Laser: %d", distancesToF[0]);
      Serial.print(characterBuffer);

      run(-120, -120);

      Serial.println();
    } while(distancesToF[0] < 230);

    run(0, 0);

    while(triangleX == 0) {
      Serial.print("Locating");
      
      if(Serial.available() > 0) { break; Serial.readString(); }
      comUpdate();
      triangleX = triangleType == 1 ? redX : greenX;

      sprintf(characterBuffer, "    x: %d y: %d r: %d Green: %d %d Red: %d %d victimType: %d", x, y, r, greenX, greenY, redX, redY, victimType);
      Serial.print(characterBuffer);
      run(150, -150);

      Serial.println();
    }

    while(triangleX > 240) {
      Serial.print("Aligning(R)");
      
      if(Serial.available() > 0) { break; Serial.readString(); }
      comUpdate();
      triangleX = triangleType == 1 ? redX : greenX;

      sprintf(characterBuffer, "    x: %d y: %d r: %d Green: %d %d Red: %d %d victimType: %d", x, y, r, greenX, greenY, redX, redY, victimType);
      Serial.print(characterBuffer);
      run(120, -120);

      Serial.println();
    }

    while(triangleX < 240) {
      Serial.print("Aligning(L)");
      
      if(Serial.available() > 0) { break; Serial.readString(); }
      comUpdate();
      triangleX = triangleType == 1 ? redX : greenX;

      sprintf(characterBuffer, "    x: %d y: %d r: %d Green: %d %d Red: %d %d victimType: %d", x, y, r, greenX, greenY, redX, redY, victimType);
      Serial.print(characterBuffer);
      run(-110, 110);

      Serial.println();
    }

    while(digitalRead(touchPins[0]) == 1 || digitalRead(touchPins[1]) == 1) {
      run(300, 300);
    }

    run(150, 150, 300);
    run(-150, -150, 500);
    run(0, 0);

    clawIncrement(1300, 1);
    delay(500);
    clawIncrement(2500, 1);
    run(0, 0, 1000);

    grabbed = false;
    victimsFound ++;
  }
}

bool grabbed = false;

void evacuation() {
  Serial.readString();

  while(Serial.available() == 0) {
    approachVictim(0.5);
    findVictim();
    grabSequence();
    findTriangle();
  }

  clawServo.writeMicroseconds(2500);
}