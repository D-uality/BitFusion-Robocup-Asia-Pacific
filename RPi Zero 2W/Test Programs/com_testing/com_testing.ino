#define com Serial1

void setup() {
  Serial.begin(115200);
  com.begin(115200);
}

void loop() {
  String data;

  while(com.available() > 0) {
    char character = com.read();
    data += character;
  }

  if(data.length() != 0) Serial.println(data);

  delay(50); // Needs fine tuning, to recieve whole messages
}
