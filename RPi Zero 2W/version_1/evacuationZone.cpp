#include <stdint.h>
#include "functions.h"
#include <Arduino.h>

void splitData() {
  int commaPositions[6];
  int commaOccurance = 0;

  for(int i=0; i<data.length(); i++) {
    if(data.charAt(i) == ',') {
      commaPositions[commaOccurance] = i;
      commaOccurance += 1;
    }
  }

  x      = data.substring(0                    , commaPositions[0]).toInt();
  y      = data.substring(commaPositions[0] + 1, commaPositions[1]).toInt();
  r      = data.substring(commaPositions[1] + 1, commaPositions[2]).toInt();
  greenX = data.substring(commaPositions[2] + 1, commaPositions[3]).toInt();
  greenY = data.substring(commaPositions[3] + 1, commaPositions[4]).toInt();
  redX   = data.substring(commaPositions[4] + 1, commaPositions[5]).toInt();
  redY   = data.substring(commaPositions[5] + 1, data.length() + 1).toInt();

  sprintf(characterBuffer, "    x: %d y: %d r: %d green: %d %d red: %d %d", x, y, r, greenX, greenY, redX, redY);
  Serial.print(characterBuffer);
}

void findVictim() {
  while(r == 0) {
    if(Serial.available() > 0) { break; Serial.readString(); }
    run(150, 150);

    Serial.print("    ");
    Serial.print(data);

    if(digitalRead(touchPins[0]) == 0 || digitalRead(touchPins[1]) == 0) {
      run(-150, -150, 700);
      run(150, -150, random(1200, 2700));
    }

    Serial.println();
  }
}

void approachVictim(float kP) {
  while(r < 90 && r != 0) {
    if(Serial.available() > 0) { break; Serial.readString(); }
    int error = x - 250;
    int turn = error * kP;
    turn = constrain(turn, -30, 30);

    sprintf(characterBuffer, "    %d | %d %d    ", error, 150 + turn, 150 - turn);
    Serial.print(characterBuffer);
    Serial.print(data);

    run(100 + turn, 100 - turn);

    Serial.println();
  }
}

void grabSequence() {
  if(r > 85) {
    run(0, 0, 500);
    clawIncrement(500, 1);

    run(100, 100, 3800);
    run(0, 0);

    clawIncrement(1200, 1);
    run(-100, -100, 3000);
    run(0, 0);

    clawIncrement(800, 1);
    delay(50);
    clawIncrement(1200, 1);
    delay(50);

    clawIncrement(1800, 2);

    grabbed = true;
  }
}

void findTriangle() {
  if(grabbed == true) {
    delay(3000);
    while(greenX == 0) {
      run(150, -150);
    }

    while(greenX > 240) {
      run(100, -100);
    }

    while(digitalRead(touchPins[0]) == 1 || digitalRead(touchPins[1]) == 1) {
      run(150, 150);
    }

    run(150, 150, 300);
    run(-150, -150, 500);
    run(0, 0);

    clawIncrement(1300, 1);
    delay(500);
    clawIncrement(2500, 1);
    run(0, 0, 5000);

    grabbed = false;
  }
}

bool grabbed = false;

void evacuation() {
  Serial.readString();

  while(Serial.available() == 0) {
    findVictim();

    run(0, 0, 500);

    while(r > 70 && r != 0) {
      run(-100, -100);
    }

    approachVictim(0.5);
    grabSequence();
    findTriangle();
  }

  clawServo.writeMicroseconds(2500);
}