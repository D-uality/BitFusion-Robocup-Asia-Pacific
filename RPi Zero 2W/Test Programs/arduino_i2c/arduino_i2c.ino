#include <Wire.h>

void setup() {
  Serial.begin(115200);
  
  Wire.begin(11);
  Wire.onReceive(receiveEvent);
}

void receiveEvent(int size) {
  Serial.println("Received Data!");
  String data = "";

  while(Wire.available() > 0) {
    char character = Wire.read();
    data += character;
  }

  Serial.print("\t");
  Serial.println(data);
}

void loop() {

}