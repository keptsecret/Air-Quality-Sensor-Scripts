#include <IoTtweet.h>

#include <ESP8266WiFi.h>

//#include <SD.h>

//#include <SPI.h>

//#include "SdFat.h"

#include "DHT.h"

#define DHTPIN 2

#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

//SdFat SD;

//File dataFile;

int pin = 2;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 30000;//sampe 30s ;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;

const int chipSelect = 10; //change accordingly

const char *userid = "000498";  
const char *key = "bmc5w8dkpy93";
const char* ssid     = "Dell_pc";
const char* password = "0853348548";

String private_tweet = "";
String public_tweet = "";
float humidity, temp, heatindex, conc, mq7, mq2;

IoTtweet myiot;

void setup() {

  Serial.begin(115200);
  String libvers = myiot.getVersion();
  Serial.println("IoTtweet Library Version: " + String(libvers));
  
  Serial.println("Air Quality Sensors test");
  pinMode(8,INPUT);
  starttime = millis();

  bool conn = myiot.begin(ssid, password);

  if (!conn) {
    Serial.println("WiFi connection failed");
  } else {
    Serial.println("WiFi connected!");
  }
  
  dht.begin();
  
  //Serial.print("Initializing SD card...");
  pinMode(chipSelect, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);  
}

void loop() {

  String dataString = "";

  float h = dht.readHumidity();
  humidity = h;
  
  float t = dht.readTemperature();
  temp = t;
  
  float mq7Value;
  float mq7Volt;
  float mq7_gas;
  float mq7_ratio;
  
  float mq2Value;
  float mq2Volt;
  float mq2_gas;
  float mq2_ratio;

  duration = pulseIn(pin, LOW);
    lowpulseoccupancy = lowpulseoccupancy+duration;
    if ((millis()-starttime) > sampletime_ms) { //if the sample time == 30s
      ratio = lowpulseoccupancy/(sampletime_ms*10.0); // Integer percentage 0=>100
      concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
      //Serial.print("concentration = ");
      //Serial.print(concentration);
      //Serial.println(" pcs/0.01cf");
      //Serial.println("\n");
      lowpulseoccupancy = 0;
      starttime = millis();

      conc = concentration;
    }

  if (isnan(h) || isnan(t)) {
    //Serial.println("Failed to read from DHT sensor");
    return;
  }

  float hic = dht.computeHeatIndex(t, h, false);
  heatindex = hic;

  mq7Value = digitalRead(1);
  mq7Volt = (float)mq7Value / 1024 * 5.0;
  mq7_gas = (5.0-mq7Volt)/mq7Volt;
  mq7_ratio = mq7_gas/0.18;
  mq7 = mq7_ratio;
  
  mq2Value = digitalRead(0);
  mq2Volt = (float)mq2Value / 1024 * 5.0;
  mq2_gas = (5.0-mq2Volt)/mq2Volt;
  mq2_ratio = mq2_gas/0.40;
  mq2 = mq2_ratio;

  String response = myiot.WriteDashboard(userid,key,humidity,temp,heatindex,conc,private_tweet,public_tweet);
  Serial.print(response);

  delay(60000); //every 1 minute
}
