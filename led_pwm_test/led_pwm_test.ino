const int pwm = 2;
void setup() {
  // put your setup code here, to run once:
  pinMode(pwm, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(pwm, random(0,255));
  delay(50);
}
