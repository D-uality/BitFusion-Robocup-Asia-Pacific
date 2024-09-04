#include <Servo.h>

const int servoPins[2] = {10, 4};
Servo servos[2];

void run(int v1, int v2) {
  int calculatedPWM;

  if     (v1 < 0) calculatedPWM = 0.60 * v1 + -88.48;
  else if(v1 > 0) calculatedPWM = 0.59 * v1 +  77.72;

  calculatedPWM = constrain(calculatedPWM, -1000, 1000);

  servos[0].writeMicroseconds(1500 + calculatedPWM);
  servos[1].writeMicroseconds(1500 + v1);
}

void setup() {
  Serial.begin(115200);

  for(int i=0; i<2; i++) {
    servos[i].attach(servoPins[i]);
  }
}

void loop() {
  Serial.println("Bot Left, Linear Test");

  for(int i=0; i<600; i++) {
    run(i, 0);
    delay(20);

    Serial.println(i);
  }

  for(int i=600; i>-600; i--) {
    run(i, 0);
    delay(20);

    Serial.println(i);
  }

  for(int i=-600; i<0; i++) {
    run(i, 0);
    delay(20);

    Serial.println(i);
  }
}