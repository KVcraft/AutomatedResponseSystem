void setup() {
  pinMode(7, OUTPUT); 
  Serial.begin(9600);
}

void loop() {
  int waterLevel = analogRead(A0);
  Serial.print("Water Level: ");
  Serial.println(waterLevel);

  if (waterLevel >= 668) {
    digitalWrite(7, HIGH);
  } else {
    digitalWrite(7, LOW);
  }
  delay(50);
}