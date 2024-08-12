#include "Functions.h"

void test() {
  analogWrite(liftClawPin, clawGrab);
  delay(3000);
  analogWrite(liftClawPin, clawUp);
  delay(3000);
}
