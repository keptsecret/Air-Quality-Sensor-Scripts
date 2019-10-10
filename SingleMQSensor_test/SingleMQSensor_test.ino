const int analogpin = A0;
const int mq2pin = 4;

void setup() {
  Serial.begin(115200);
  // declare the enable and ledPin as an OUTPUT:
  pinMode(mq2pin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  float mq2_value;
  float mq2_volt;
  float mq2_gas;
  float mq2_ratio;
  float mq2_hold;
  float mq2_ppm;
  //Serial.print("reading mq2...");
  //digitalWrite(mq2pin, HIGH);
  mq2_value = analogRead(analogpin);
  //digitalWrite(mq2pin, LOW);
  //Serial.println("done!");
  mq2_volt = (float)mq2_value / 1024 * 5.0;
  mq2_gas = (5.0-mq2_volt)/mq2_volt;
  mq2_ratio = mq2_gas/0.40;
  mq2_hold = 16.177347074757936/(mq2_ratio+1.1016286306095602);
  mq2_ppm = pow(mq2_hold, (1/0.245));
  Serial.println(mq2_ratio);
  Serial.println(mq2_ppm);
  Serial.println("----------------------------------------");
  delay(2000);
}
