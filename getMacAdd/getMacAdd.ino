#include <ESP8266WiFi.h>

void setup() {
  Serial.begin(115200);

  delay(500);

  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());

}

void loop() {
  // put your main code here, to run repeatedly:

}
