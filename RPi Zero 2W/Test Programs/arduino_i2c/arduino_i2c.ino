#include <Wire.h>
long start;

void setup() {
  Serial.begin(115200);
  
  Wire.begin(11);
  Wire.onReceive(receiveEvent);
}4

void receiveEvent(int size) {
  Serial.println("Received Data!");
  String data = "";

  while(Wire.available() > 0) {
    char character = Wire.read();
    data += character;
  }

  Serial.print("\t");
  Serial.println(data);

  start = millis();
}

void loop() {
  if(millis() - start > 1) {
    start = millis();
    Serial.println("RUNNING");
  }
}