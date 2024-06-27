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

    ToF[0].rangingTest(&distancesToF[0], false);
    ToF[1].rangingTest(&distancesToF[1], false);
    ToF[2].rangingTest(&distancesToF[2], false);
    
    sprintf(characterBuffer, "    TOF | Front: %d Left: %d Right: %d", distancesToF[0].RangeMilliMeter, distancesToF[1].RangeMilliMeter, distancesToF[2].RangeMilliMeter);
    Serial.print(characterBuffer);
  }

  else if(programMode == 't') {
    Serial.print("    Testing");

    clawIncrement(500, 1);
    clawIncrement(1100, 1);
    clawIncrement(1700, 1);
    clawIncrement(2500, 1);
  }

  else {
    Serial.print("Unrecognised!");
    programMode = 's';
  }

  Serial.println();
}
