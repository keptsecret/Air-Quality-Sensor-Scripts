/*

  Multiple Analog inputs on one Analoge pin

  Show input of two sensors on one analog pin.
  The vaues of both sensors will be displayed in the serial monitor.

  The circuit:
   center pin of both Potentiometer/Sensors attached via a diode to analog input 0
   one side pin (either one)of both potentiometers/sensors to ground
   the other side of both potentiometers/sensors pin to +5V
   pull-down resitor (10k) between analog pin 0 and ground


  Created by Vincent Verheggen
  Ingenerare

  based on:
  example sketch analog input by David Cuartielles
  http://www.arduino.cc/en/Tutorial/AnalogInput
  sketch breagan 22
  http://www.instructables.com/id/ESP8266-with-Multiple-Analog-Sensors/

*/

const int analogpin = A0;
const int mq2pin = 4;
const int mq7pin = 5;

void setup() {


  Serial.begin(115200);
  // declare the enable and ledPin as an OUTPUT:
  pinMode(mq2pin, OUTPUT);
  pinMode(mq7pin, OUTPUT);
}

void loop() {
  //read from mq sensors

  float mq7_value;
  float mq7_volt;
  float mq7_gas;
  float mq7_ratio;
  float mq7_hold;
  float mq7_ppm;
  //Serial.print("reading mq7...");
  digitalWrite(mq7pin, HIGH);
  delay(1000 * 5);
  mq7_value = analogRead(analogpin);
  digitalWrite(mq7pin, LOW);
  //Serial.print("done!");
  mq7_volt = (float)mq7_value / 1024 * 5.0;
  mq7_gas = (5.0 - mq7_volt) / mq7_volt;
  mq7_ratio = mq7_gas / 0.18;
  mq7_hold = 24.671324557143727 / (mq7_ratio - 0.021209244624595787);
  mq7_ppm = pow(mq7_hold, (1 / 0.7));
  Serial.println(mq7_ratio);
  Serial.println(mq7_ppm);

  delay(100);

  float mq2_value;
  float mq2_volt;
  float mq2_gas;
  float mq2_ratio;
  float mq2_hold;
  float mq2_ppm;
  //Serial.print("reading mq2...");
  digitalWrite(mq2pin, HIGH);
  delay(1000 * 5);
  mq2_value = analogRead(analogpin);
  digitalWrite(mq2pin, LOW);
  //Serial.println("done!");
  mq2_volt = (float)mq2_value / 1024 * 5.0;
  mq2_gas = (5.0 - mq2_volt) / mq2_volt;
  mq2_ratio = mq2_gas / 0.40;
  mq2_hold = 16.177347074757936 / (mq2_ratio + 1.1016286306095602);
  mq2_ppm = pow(mq2_hold, (1 / 0.245));
  Serial.println(mq2_ratio);
  Serial.println(mq2_ppm);
  Serial.println("----------------------------------------");
  delay(2000);
}
