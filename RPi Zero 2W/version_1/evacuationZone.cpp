#include <stdint.h>
#include "functions.h"
#include <Arduino.h>

void splitData() {
  int commaPositions[2] = {0, 0};
  int commaOccurance = 0;

  for(int i=0; i<data.length(); i++) {
    if(data.charAt(i) == ',') {
      commaPositions[commaOccurance] = i;
      commaOccurance += 1;
    }
  }

  x = data.substring(0                    , commaPositions[0]).toInt();
  y = data.substring(commaPositions[0] + 1, commaPositions[1]).toInt();
  r = data.substring(commaPositions[1] + 1, data.length() + 1).toInt();

  sprintf(characterBuffer, "    x: %d y: %d r: %d", x, y, r);
  Serial.print(characterBuffer);
}



void evacuation() {
  while(r == 0) {
    run(120, 120);
  }

  run(0, 0);
  Serial.readString();
  while(Serial.available() == 0) { }

  sprintf(characterBuffer, "    x: %d y: %d r: %d", x, y, r);
  Serial.print(characterBuffer);
}