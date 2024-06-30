const int SensorPin = A0;

void setup() {
	Serial.begin(115200);

	pinMode(SensorPin, INPUT);
}

void loop() {
	Serial.println(analogRead(SensorPin));
}