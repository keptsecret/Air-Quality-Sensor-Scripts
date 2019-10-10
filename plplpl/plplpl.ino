void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int p = analogRead(A0);
  //Serial.println(p);
  delay(500);

  if (p>700) {
    Serial.println("SF&ssid=Dell_pc&psk=0853348548");
  }
}
