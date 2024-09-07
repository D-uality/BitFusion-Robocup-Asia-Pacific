#include <Servo.h>

const int servoPins[4] = {10, 4, 9, 12};
Servo servos[4];

void run(int v1, int v2) {
  int calculatedPWM[4] = {0, 0, 0, 0};

  float gradientNegative[4]    = {0.60, 0.56, 0.58, 0.61};
  float yInterceptsNegative[4] = {-88.48, -66.02, -74.74, -70.33};
  float gradientPositive[4]    = {0.59, 0.57, 0.55, 0.57};
  float yInterceptsPositive[4] = {77.72, 42.17, 73.16, 62.34};
  
  for(int i=0; i<2; i++) {
    if      (v1 < yInterceptsNegative[i]) calculatedPWM[i] = gradientNegative[i] * v1 + yInterceptsNegative[i];
    else if (v1 > yInterceptsPositive[i]) calculatedPWM[i] = gradientPositive[i] * v1 + yInterceptsPositive[i];
    else                                  calculatedPWM[i] = 0;
    
    calculatedPWM[i] = constrain(calculatedPWM[i], -500, 500);
  }

  v2 *= -1;
  for(int i=2; i<4; i++) {
    if      (v2 < yInterceptsNegative[i]) calculatedPWM[i] = gradientNegative[i] * v2 + yInterceptsNegative[i];
    else if (v2 > yInterceptsPositive[i]) calculatedPWM[i] = gradientPositive[i] * v2 + yInterceptsPositive[i];
    else                                  calculatedPWM[i] = 0;
    
    calculatedPWM[i] = constrain(calculatedPWM[i], -500, 500);
  }

  servos[0].writeMicroseconds(1500 + calculatedPWM[0]);
  servos[1].writeMicroseconds(1500 + calculatedPWM[1]);
  servos[2].writeMicroseconds(1500 + calculatedPWM[2]);
  servos[3].writeMicroseconds(1500 + calculatedPWM[3]);
}

void setup() {
  Serial.begin(115200);

  for(int i=0; i<4; i++) {
    servos[i].attach(servoPins[i]);
  }
}

void loop() {
  Serial.println("Bot Left, Linear Test");
  run(300, 300);

  // for(int i=0; i<600; i++) {
  //   run(i, 0);
  //   delay(20);

  //   Serial.println(i);
  // }

  // for(int i=600; i>-600; i--) {
  //   run(i, 0);
  //   delay(20);

  //   Serial.println(i);
  // }

  // for(int i=-600; i<0; i++) {
  //   run(i, 0);
  //   delay(20);

  //   Serial.println(i);
  // }
  
  // Serial.println("Top Right, + 400");
  // servos[3].writeMicroseconds(1500 + 400);
}