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

void evacuation() {
  Serial.readString();
  while(r == 0) {
    if(Serial.available() > 0) { break; Serial.readString(); }
    run(150, 150);

    sprintf(characterBuffer, "    x: %d y: %d r: %d", x, y, r);
    Serial.print(characterBuffer);

    if(digitalRead(touchPins[0]) == 0 || digitalRead(touchPins[1]) == 0) {
      run(-150, -150, 700);
      run(150, -150, random(1200, 2700));
    }

    Serial.println();
  }

  run(0, 0, 500);

  while(r > 70) {
    run(-100, -100);
  }

  while(r < 90) {
    if(Serial.available() > 0) { break; Serial.readString(); }
    int error = x - 250;
    int turn = error * 0.5;
    turn = constrain(turn, -30, 30);

    sprintf(characterBuffer, "    %d | %d %d", error, 150 + turn, 150 - turn);
    Serial.println(characterBuffer);

    run(100 + turn, 100 - turn);
  }
  
  run(0, 0, 500);
  clawServo.writeMicroseconds(500);
  delay(1000);
  run(100, 100, 3500);
  run(0, 0);
  clawIncrement(1200, 1);
  run(-100, -100, 1000);
  run(0, 0);
  clawIncrement(1400, 2);

  while(Serial.available() == 0) { }
  Serial.readString();

  clawServo.writeMicroseconds(2500);

  sprintf(characterBuffer, "    x: %d y: %d r: %d", x, y, r);
  Serial.print(characterBuffer);
}