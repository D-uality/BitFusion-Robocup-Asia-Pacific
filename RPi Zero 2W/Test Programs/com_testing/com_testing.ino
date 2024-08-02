#define com Serial1

int x, y, r;
int expectedCommaCount = 2;
long lastUpdate = millis();
String data;
char characterBuffer[100];

void comUpdate() {
  if(com.available() && millis() - lastUpdate > 7) {
    data = "";
    while(com.available() > 0) {
      char character = com.read();
      data += character;
    }

    int commaCount = 0;
    for(int i=0; i<data.length(); i++) { if(data.charAt(i) == ',') commaCount ++; }
    if(commaCount == expectedCommaCount) splitData();

    lastUpdate = millis();
  }

  sprintf(characterBuffer, "    Victim: %d %d %d", x, y, r);
  Serial.print(characterBuffer);
}

void splitData() {
  int commaPositions[expectedCommaCount];
  int commaOccurance = 0;

  for(int i=0; i<data.length(); i++) {
    if(data.charAt(i) == ',') {
      commaPositions[commaOccurance] = i;
      commaOccurance += 1;
    }
  }

  x            = data.substring(0                    , commaPositions[0]).toInt();
  y            = data.substring(commaPositions[0] + 1, commaPositions[1]).toInt();
  r            = data.substring(commaPositions[1] + 1, commaPositions[2]).toInt();
}

void setup() {
  Serial.begin(115200);
  com.begin(115200);
}

void loop() {
  comUpdate();

  Serial.println();
}

