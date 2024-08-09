#include <Servo.h>

Servo servos[4];
const int driveServoPins[4] = {10, 11, 9, 12};

void setup() {
  Serial.begin(115200);

  for(int i=0; i<4; i++) {
    servos[i].attach(driveServoPins[i]);
  }


}

// 0: 1380 to 1630
// 1: 1400 to 1550
// 2: 1380 to 1600
// 3: 1350 to 1680

const int MID[4] = {1505, 1475, 1490, 1515};

void loop() {
  for(int i=1000; i<2000; i+=2) {
    Serial.println(i);

    servos[3].writeMicroseconds(i);

    delay(50);
  }
}
