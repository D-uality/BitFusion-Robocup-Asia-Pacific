//  ----------------------------------------  VERSION: FULL GRIPPY TIRES

#include "Functions.h"

int mode = 's';
const int xShutPins[3] = {31, 30, 32};
const int touchPins[2] = {33, 29};
const int resetPin = 28;

void setup() {
  Serial.begin(115200);

  //  Reset Pin Setup
  pinMode(resetPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(resetPin), resetISR, FALLING);

  analogReadAveraging(32);          //  Average 32 readings to reduce sensor noise

  //  Set PWM frequency to 330Hz for the servo pins
  analogWriteFrequency(frontLeftPin, frequency);
  analogWriteFrequency(frontRightPin, frequency);
  analogWriteFrequency(backLeftPin, frequency);
  analogWriteFrequency(backRightPin, frequency);
  analogWriteFrequency(liftClawPin, frequency);
  analogWriteFrequency(grabClawPin, frequency);
  analogWrite(liftClawPin, clawUp);
  analogWrite(grabClawPin, clawGrab);

  //  Setup Touch Sensor
  for(int sensorNum = 0; sensorNum < 2; sensorNum++) pinMode(touchPins[sensorNum], INPUT_PULLUP);

  // Setup Laser Sensor
  Wire.begin();
  for(int sensorNum = 0; sensorNum < 3; sensorNum++) {
    pinMode(xShutPins[sensorNum], OUTPUT);
    digitalWrite(xShutPins[sensorNum], HIGH);
    
    if (!laser[sensorNum].init()) {
      Serial.print("Laser sensor ");
      Serial.print(sensorNum);
      Serial.println(" failed to initialize");
    } 
    else {
      laser[sensorNum].setAddress(0x30 + sensorNum * 2);
      laser[sensorNum].setTimeout(500);
      laser[sensorNum].setMeasurementTimingBudget(50000);
      Serial.print("Laser sensor ");
      Serial.print(sensorNum);
      Serial.println(" initialized successfully");
    }
  }

  //  Reading Calibrated values from EEPROM
  int address = 0;
  for(int sensorNum = 0; sensorNum < 6; sensorNum++) {
    prevCalibratedMin[sensorNum] = 10*EEPROM.read(address++);
    calibratedMin[sensorNum] = prevCalibratedMin[sensorNum];
  }

  for(int sensorNum = 0; sensorNum < 6; sensorNum++) {
    prevCalibratedMax[sensorNum] = 10*EEPROM.read(address++);
    calibratedMax[sensorNum] = prevCalibratedMax[sensorNum];
  }
}

void loop() {
  // Read Serial for instructions
  if (Serial.available() > 0) { 
    Serial.println("Reading for instructions: ");
    mode = Serial.read();
    while(Serial.available() > 0) Serial.read();
    Serial.println((char)mode);
  }

  // Switching between different modes
  switch(mode) {
    case 'g': running(); break;
    case 'r': 
      runMotors(0, 0, 0);
      readColourSensors(1); 
      break;
    case 't': 
      runMotors(0, 0, 0);
      readTouchSensors(1); 
      break;
    case 'l':
      runMotors(0, 0, 0);
      readLaserSensors(1); 
      break;
    case 'c':
      autoCalibration(1);
    case 's': 
      Serial.println("---------- Command Line ----------");
      Serial.println("g = Run Program");
      Serial.println("r = Read Colour Sensors");
      Serial.println("t = Read Touch Sensors");
      Serial.println("l = Read Laser Sensors");
      Serial.println("c = Calibrate Sensors");
      Serial.println("s = Stop");
      Serial.println("i = Test");
      autoCalibration(0);
      analogWrite(liftClawPin, clawUp);
      analogWrite(grabClawPin, clawGrab);
      runMotors(0, 0, 0);
      mode = 0;
      break;
    case 'i': test(); break;
    case 0: break;
  }
}

//  ----------------------------------------  Reset Pin Function
volatile unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 25;

void resetISR() {
  unsigned long currentTime = millis();  // Get the current time
  if ((currentTime - lastDebounceTime) > debounceDelay) {  // Check if the debounce delay has passed
    lastDebounceTime = currentTime;  //  Update the last debounce time
    Serial.println("Reset Found");
    SCB_AIRCR = 0x05FA0004; // Trigger a system reset
    delay(100);
  }
}
