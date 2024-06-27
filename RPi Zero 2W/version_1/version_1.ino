#include "functions.h"
#include <Arduino.h>

const int ARDIONO_ADDRESS = 11;
byte programMode = 's';
char Buffer[100];

void setup() {
  Serial.begin(115200);
  Wire.begin(ARDIONO_ADDRESS);

  setupDriveServos([10, 11, 9, 12]);
}

void loop() {
  if(Serial.available() > 1) programMode = Serial.read();

  // ------------------------------------------------------------- LINE BREAK -------------------------------------------------------------

  Serial.print(programMode);

  if(programMode == 's') {
    Serial.print("    Progarm stopped!");
    Run(0, 0);
  }

  else if(programMode == 'g') {
    Serial.println("    Main program started!");
    Run(100, 100, 1000);
    Run(0, 0, 1000);
  }

  Serial.println();
}
