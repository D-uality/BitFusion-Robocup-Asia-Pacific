const int colorSensorPins[6] = {A3, A5, A7, A9, A11, A13};
/*
     A11            A13

A3     A5         A7     A9
*/

void setup() {
  Serial.begin(115200);

  for(int i=0; i<6; i++) {
    pinMode(colorSensorPins[i], INPUT);
  }
}

void loop() {
  int colorsensorValues[6];

  for(int i=0; i<6; i++) {
    colorsensorValues[i] = analogRead(colorSensorPins[i]);
    Serial.print(colorsensorValues[i]);
    Serial.print("    ");
  }

  Serial.println();

  delay(50);
}
