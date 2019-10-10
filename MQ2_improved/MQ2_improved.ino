void setup() {
    Serial.begin(115200);
}

void loop() {

  float mq2_value;
  float mq2_volt;
  float mq2_gas;
  float mq2_ratio;
  float mq2_hold;
  float mq2_ppm;
  mq2_value = analogRead(A0);
  mq2_volt = (float)mq2_value / 1024 * 5.0;
  mq2_gas = (5.0-mq2_volt)/mq2_volt;
  mq2_ratio = mq2_gas/0.40;
  mq2_hold = 16.177347074757936/(mq2_ratio+1.1016286306095602);
  mq2_ppm = pow(mq2_hold, (1/0.245));

          /*-Replace the name "R0" with the value of R0 in the demo of First Test -*/
    //ratio = RS_gas/0.18;  // ratio = RS/R0 (R0 = 0.18)
          /*-----------------------------------------------------------------------*/
  Serial.println(mq2_ratio);
  Serial.print(mq2_ppm);

  Serial.print("\n\n");

    delay(2000);
}
