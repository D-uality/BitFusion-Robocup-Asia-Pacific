#include "functions.h"
#include <Arduino.h>

const int ARDUINO_ADDRESS = 0x0b;                                                                                         // The I2C address to appear as for the Rasbperry Pi to establish communications
byte programMode = 's';                                                                                                 // Current mode that the arduinp is in
char characterBuffer[100];                                                                                              // Character buffer, used for sprintf()
unsigned long programCount = 0;
String data;
int x, y, r;

void receiveEvent(int size) {
  Serial.println("Received Data!");
  data = "";

  while(Wire.available() > 0) {
    char character = Wire.read();
    data += character;
  }

  data.remove(0, 1);

  Serial.print("\t");
  Serial.println(data);

  splitData();
}

void setup() {
  Serial.begin(115200);

  Serial.println("\nCheckup on all devices ...");
  
  setupDriveServos();
  clawServo.attach(clawServoPin);
  clawServo.writeMicroseconds(2500);
  setupToF();
  setupTouch();
  setupColorSensors();
  readFromEEPROM();

  Serial.println("\nWaiting for key press ... ");
  while(Serial.available() == 0) {}

  Wire.begin(ARDUINO_ADDRESS);
  Wire.onReceive(receiveEvent);
}

void loop() {
  if(Serial.available() > 1) programMode = Serial.read();
  Serial.print(programMode);

  if(programMode == 's') {
    Serial.print("    Stopped");
    run(0, 0);
    clawServo.writeMicroseconds(2500);
  }

  else if(programMode == 'g') {
    Serial.print("    Running");

    running();
  }

  else if(programMode == 'c') {
    Serial.print("    Calibrating");
    resetCalibrationValues();
    run(0, 0);
    Serial.readString();
    while(Serial.available() == 0) { calibrateColorSensors(); Serial.println(); }
  }

  else if(programMode == 'w') {
    Serial.print("    Writing to EEPROM");

    convertToBytes();
    writeToEEPROM();

    Serial.readString();
    while(Serial.available() == 0) {}

    readFromEEPROM();
    programMode = 's';
  }

  else if(programMode == 'r') {
    Serial.print("    Reading");

    sprintf(characterBuffer, "    Left: %d | Right: %d", digitalRead(touchPins[0]), digitalRead(touchPins[1]));
    Serial.print(characterBuffer);

    for(int i=0; i<3; i++) {
      if(ToF[i].isRangeComplete()) distancesToF[i] = ToF[i].readRange();
    }
    
    sprintf(characterBuffer, "    TOF | Front: %d Left: %d Right: %d", distancesToF[0], distancesToF[1], distancesToF[2]);
    Serial.print(characterBuffer);

    readColorSensors();
  }

  else if(programMode == 'e') {
    Serial.print("    Evacuation");

    evacuation();
  }

  else {
    Serial.print("Unrecognised!");
    programMode = 's';
  }

  Serial.println();
}
