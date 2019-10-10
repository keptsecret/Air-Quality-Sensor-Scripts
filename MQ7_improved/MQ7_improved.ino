void setup() {
    Serial.begin(115200);
}

void loop() {
  float mq7_value;
  float mq7_volt;
  float mq7_gas;
  float mq7_ratio;
  float mq7_hold;
  float mq7_ppm;
  Serial.print("reading mq7...");
  mq7_value = analogRead(A0);
  //Serial.print("done!");
  mq7_volt = (float)mq7_value / 1024 * 5.0;
  mq7_gas = (5.0-mq7_volt)/mq7_volt;
  mq7_ratio = mq7_gas/0.18;
  mq7_hold = 24.671324557143727/(mq7_ratio-0.021209244624595787);
  mq7_ppm = pow(mq7_hold, (1/0.7));

          /*-Replace the name "R0" with the value of R0 in the demo of First Test -*/
    //ratio = RS_gas/0.18;  // ratio = RS/R0 (R0 = 0.18)
          /*-----------------------------------------------------------------------*/
  Serial.println(mq7_value);
  Serial.println(mq7_volt);
  Serial.println(mq7_ratio);
  Serial.print(mq7_ppm);

    Serial.print("\n\n");

    delay(2000);

}
