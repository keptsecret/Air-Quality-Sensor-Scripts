void setup() {
  Serial.begin(115200);
  pinMode(5, OUTPUT);
}

void loop() {
  digitalWrite(5, HIGH);
  delay(1000 * 5);
  float mq7_value = analogRead(A0);
  //digitalWrite(4, LOW);
  float mq7_volt = (float)mq7_value / 1024 * 5.0;
  float mq7_gas = (5.0 - mq7_volt) / mq7_volt;
  float mq7_ratio = mq7_gas / 0.18;
  float mq7_hold = 24.671324557143727 / (mq7_ratio - 0.021209244624595787);
  float mq7_ppm = pow(mq7_hold, (1 / 0.7));
  Serial.println(mq7_ratio);
  Serial.println(mq7_ppm);
  delay(2000);
}
