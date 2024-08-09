#include <stdint.h>
#include "functions.h"
#include <Arduino.h>

void read() {
  Serial.print("    Reading");

  readToF(0);

  
}