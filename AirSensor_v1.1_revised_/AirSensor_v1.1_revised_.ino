/* For use with new IoTtweet API revised 2
 *  Status: complete (pretty much)
 *  humidity and heatindex sensors have been blotted out for the time being
 */

#include <IoTtweet.h>
#include <ESP8266WiFi.h>
#include "DHT.h"

#define DHTPIN 2 //actually D4 on nodemcu
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

//pins
const int dhtpin = 2;
const int dustpin = 3; //actually D0 on nodemcu
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
const char* userid = "000498";  
const char* key = "bmc5w8dkpy93";
const char* ssid     = "Dell_pc";
const char* password = "0853348548";

//to IoTtweet
String private_tweet = "";
String public_tweet = "";
float humidity, temp, heatindex, conc, mq7, mq2;
IoTtweet iot;

void setup() {

  //begin
  //Serial.begin(115200);
  String libvers = iot.getVersion();
  //Serial.println("IoTtweet Library Version: " + String(libvers));

  //Serial.println("Air Quality Sensors test");

  //enable mq sensors
  pinMode(mq2, OUTPUT);
  pinMode(mq7, OUTPUT);
  
  //connect to wifi
  //Serial.print("Connecting to WiFi... ");
  bool conn = iot.begin(ssid, password);

  /*
  if (!conn) {
    Serial.println("Connection failed.");
  } else {
    Serial.println("Connected!");
  }*/

  //begin temperature sensor
  dht.begin();

}

void loop() {

  delay(60000); //every 1 minute

  //read from DHT22
  float h = dht.readHumidity();
  humidity  = h;
  float t = dht.readTemperature();
  temp = t;

  if (isnan(h) || isnan(t)){
    //Serial.println("Failed to read from DHT sensor");
    return;
  }
  
  float hic = dht.computeHeatIndex(t, h, false);
  heatindex = hic;

  //read from mq sensors
  
  float mq7_value;
  float mq7_volt;
  float mq7_gas;
  float mq7_ratio;
  //Serial.print("reading mq7...");
  digitalWrite(mq7pin, HIGH);
  mq7_value = analogRead(analogpin);
  digitalWrite(mq7pin, LOW);
  //Serial.print("done!");
  mq7_volt = (float)mq7_value / 1024 * 5.0;
  mq7_gas = (5.0-mq7_volt)/mq7_volt;
  mq7_ratio = mq7_gas/0.18;
  float mq7 = mq7_ratio;

  delay(100);

  float mq2_value;
  float mq2_volt;
  float mq2_gas;
  float mq2_ratio;
  //Serial.print("reading mq2...");
  digitalWrite(mq2pin, HIGH);
  mq2_value = analogRead(analogpin);
  digitalWrite(mq2pin, LOW);
  //Serial.println("done!");
  mq2_volt = (float)mq2_value / 1024 * 5.0;
  mq2_gas = (5.0-mq2_volt)/mq2_volt;
  mq2_ratio = mq2_gas/0.40;
  float mq2 = mq2_ratio;
  
  
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
      //Serial.println(concentration);
    }

  /*
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print("%\t");
  Serial.print("Temp.: ");
  Serial.print(t);
  Serial.print("*C\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.println("*C");

  Serial.print("Concentration: ");
  Serial.print(conc);
  Serial.println("\t");
  
  //Serial.print("MQ2 Volt: ");
  //Serial.print(mq2Volt);
  //Serial.print("V\t");
  Serial.print("MQ2 RS/R0: ");
  Serial.println(mq2_ratio);
  
  //Serial.print("MQ7 Volt: ");
  //Serial.print(mq7Volt);
  //Serial.print("V\t");
  Serial.print("MQ7 RS/R0: ");
  Serial.print(mq7_ratio);
  Serial.println("\t");
  Serial.println(" ");*/
 
  //send data to IoTtweet dashboard
  String response = iot.WriteDashboard(userid, key, temp, mq2, mq7, conc, private_tweet, public_tweet);
  Serial.print(response);
}
