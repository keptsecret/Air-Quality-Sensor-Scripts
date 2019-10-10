#include <dht22.h>

void setup() {
  Serial.begin(115200);

  DHT_init(2,DHT22,28);
  DHT_begin();
}

void loop() {
  delay(2000);

  float t = readTemperature(false);
  float h = readHumidity();

  Serial.print(t);
  Serial.print(" ");
  Serial.print(h);

}
