/* For use with ThingSpeak API
 *  Status: working
 *  For v1 pcbs where pins for dust and dht22 were swapped
 */

#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include "DHT.h"

#define DHTPIN 16 //actually D4 on nodemcu
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

//pins
const int dustpin = 2; //actually D0 on nodemcu
const int analogpin = A0;
const int mq2pin = 4;
const int mq7pin = 5;

//dust sensor vars
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 30000;//sample 30s ;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;

//wifi credentials
unsigned long channelNumber = 313398;
const char* key             = "IBAAWCZSJ2WTQD9T"; // write key
const char* ssid            = "Dell_pc";          // MUST CHANGE THESE TWO!!!!!
const char* password        = "0853348548";                     

WiFiClient client;

void setup() {

  //begin
  //Serial.begin(115200);

  //Serial.println("Air Quality Sensors test");

  //enable mq sensors
  pinMode(mq2pin, OUTPUT);
  pinMode(mq7pin, OUTPUT);

  //enable dust sensor
  pinMode(dustpin, INPUT);
  starttime = millis();
  
  //connect to wifi
  //Serial.print("Connecting to WiFi... ");
  WiFi.begin(ssid, password);

  //begin temperature sensor
  dht.begin();

  ThingSpeak.begin(client);

}

void loop() {

  delay(60000); //every 1 minute(s) times 5

  //read from DHT22
  float h = dht.readHumidity();
  ThingSpeak.setField(2,h);
  float t = dht.readTemperature();
  ThingSpeak.setField(1,t);
  
  float hic = dht.computeHeatIndex(t, h, false);
  ThingSpeak.setField(3,hic);

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
  mq7_gas = (5.0-mq7_volt)/mq7_volt;
  mq7_ratio = mq7_gas/0.18;
  mq7_hold = 24.671324557143727/(mq7_ratio-0.021209244624595787);
  mq7_ppm = pow(mq7_hold, (1/0.7));
  ThingSpeak.setField(4,mq7_ppm);

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
  mq2_gas = (5.0-mq2_volt)/mq2_volt;
  mq2_ratio = mq2_gas/0.40;
  mq2_hold = 16.177347074757936/(mq2_ratio+1.1016286306095602);
  mq2_ppm = pow(mq2_hold, (1/0.245));
  ThingSpeak.setField(5,mq2_ppm);
  
  
  //read from dust sensor
  duration = pulseIn(dustpin, LOW);
  float conc;
    lowpulseoccupancy = lowpulseoccupancy+duration;
    if ((millis()-starttime) > sampletime_ms) { //if the sample time == 30s
      ratio = lowpulseoccupancy/(sampletime_ms*10.0); // Integer percentage 0=>100
      concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
      lowpulseoccupancy = 0;
      starttime = millis();

      conc = concentration;
      ThingSpeak.setField(6,conc);
      //Serial.println(concentration);
    }
  /*
  Serial.println(mq2_value);
  Serial.println(mq2_ratio);
  Serial.println(mq2_ppm);
  Serial.println(mq7_value);
  Serial.println(mq7_ratio);
  Serial.println(mq7_ppm);*/

  //Write fields to ThingSpeak
  ThingSpeak.writeFields(channelNumber, key);
}
