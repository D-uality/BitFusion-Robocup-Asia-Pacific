#include "functions.h"
#include <Arduino.h>

const int ARDIONO_ADDRESS = 11;                                         // The I2C address to appear as for the Rasbperry Pi to establish communications
byte programMode = 's';                                                 // Current mode that the arduinp is in
char characterBuffer[100];                                              // Character buffer, used for sprintf()

void setup() {
  Serial.begin(115200);
  Wire.begin(ARDIONO_ADDRESS);

  Serial.println("Checkup on all devices ...");
  
  setupDriveServos();
  clawServo.attach(clawServoPin);
  clawServo.writeMicroseconds(2500);
  setupToF();
  setupTouch();

  Serial.println("\nWaiting for key press ... ");
  while(Serial.available() == 0) {}
  Serial.readString();
}

void loop() {
  if(Serial.available() > 1) programMode = Serial.read();
  Serial.print(programMode);

  if(programMode == 's') {
    Serial.print("    Stopped");
    Run(0, 0);
    clawServo.writeMicroseconds(2500);
  }

  else if(programMode == 'g') {
    Serial.print("    Running");

    for(int i=0; i<3; i++) {
      if(ToF[i].isRangeComplete()) distancesToF[i] = ToF[i].readRange();
    }
    
    sprintf(characterBuffer, "    TOF | Front: %d Left: %d Right: %d", distancesToF[0], distancesToF[1], distancesToF[2]);
    Serial.print(characterBuffer);
  }

  else if(programMode == 't') {
    Serial.print("    Testing");

    sprintf(characterBuffer, "    Left: %d | Right: %d", digitalRead(touchPins[0]), digitalRead(touchPins[1]));
    Serial.print(characterBuffer);
  }

  else {
    Serial.print("Unrecognised!");
    programMode = 's';
  }

  Serial.println();
}
