#include <stdint.h>
#include "functions.h"
#include <Arduino.h>

void testing() {
  Serial.print("    Testing");

  run(160, 160, 3000);
  run(-160, -160, 3000);
  run(120, 120, 3000);
  run(-120, -120, 3000);
}