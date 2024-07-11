#define com Serial1

void setup() {
  Serial.begin(115200);
  com.begin(115200);
}

void loop() {
  String data = "_";

  while(com.available() > 0) {
    data += com.read();
  }

  Serial.print(data);
}
