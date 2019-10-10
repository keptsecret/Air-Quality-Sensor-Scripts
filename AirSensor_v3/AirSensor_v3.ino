/* Air Quality Sensor v3
 *  for using with netpie.io
 *  Work in progress
 *  
 *  TODO #1 change time intervals (perhaps)
 *  TODO #2 save data on some place
 */

#include <MicroGear.h>
#include <ESP8266WiFi.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

//pins
const int dhtpin = 2;
const int dustpin = 3;
const int analogpin = A0;
const int mq2pin = 4;
const int mq7pin = 5;

//dust sensor vars
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 30000;//sampe 30s ;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;

//wifi credentials
#define APPID   "AirSensorPrototype"
#define KEY     "3LyDlYWe4KhPnj2"
#define SECRET  "FKpkm0ZxvKPvDR8uQPRLE960a"
#define ALIAS   "esp8266"
const char* ssid     = "Dell_pc";
const char* password = "0853348548";

WiFiClient client;
int timer = 0;
MicroGear microgear(client);

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
    Serial.print("Incoming message -->");
    msg[msglen] = '\0';
    Serial.println((char *)msg);
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.println("Connected to NETPIE...");
    microgear.setAlias(ALIAS);
}

void setup() {
  //begin
  dht.begin();

  microgear.on(MESSAGE, onMsghandler);
  microgear.on(MESSAGE, onConnected);

  Serial.begin(115200);
  Serial.println("Starting...");

  //connect to wifi
  if (WiFi.begin(ssid, password)) {
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
  }

  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //start microgear
  microgear.init(KEY,SECRET,ALIAS);
  microgear.connect(APPID);
}

void loop() {

  delay(100); //every 10 seconds
  
  if (microgear.connected()) {
    Serial.println("connected");
    microgear.loop();

    if (timer >= 1000) {
      String dataJson = "";
      
      //read from DHT22
      float h = dht.readHumidity();
      dataJson += String(h);
  
      float t = dht.readTemperature();
      dataJson += String(",");
      dataJson += String(t);
  
      if (isnan(h) || isnan(t)){
        Serial.println("Failed to read from DHT sensor");
        return;
      }
  
      float hic = dht.computeHeatIndex(t, h, false);
      dataJson += String(",");
      dataJson += String(hic);

      //read from mq sensors
  
      float mq7_value;
      float mq7_volt;
      float mq7_gas;
      float mq7_ratio;
      //Serial.print("reading mq7...");
      digitalWrite(mq7pin, HIGH);
      mq7_value = analogRead(analogpin);
      digitalWrite(mq7pin, LOW);
      //Serial.println("done!");
      mq7_volt = (float)mq7_value / 1024 * 5.0;
      mq7_gas = (5.0-mq7_volt)/mq7_volt;
      mq7_ratio = mq7_gas/0.18;
      float mq7 = mq7_ratio;
      dataJson += String(",");
      dataJson += String(mq7);

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
      dataJson += String(",");
      dataJson += String(mq2);
  
      //read from dust sensor
      duration = pulseIn(dustpin, LOW);
        lowpulseoccupancy = lowpulseoccupancy+duration;
        if ((millis()-starttime) > sampletime_ms) { //if the sample time == 30s
          ratio = lowpulseoccupancy/(sampletime_ms*10.0); // Integer percentage 0=>100
          concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
          lowpulseoccupancy = 0;
          starttime = millis();

        dataJson += String(",");
        dataJson += String(concentration);
      }

      //combine all data into string with sprintf
      //sprintf(data, h,t,hic,mq7,mq2,concentration);
      Serial.println(dataJson);
  
      //send data to netpie freeboard
      Serial.print("Sending -->");
      
      char topic_temp[MAXTOPICSIZE];
      char topic_humid[MAXTOPICSIZE];
      char topic_hic[MAXTOPICSIZE];
      char topic_mq7[MAXTOPICSIZE];
      char topic_mq2[MAXTOPICSIZE];
      char topic_dust[MAXTOPICSIZE];

      sprintf(topic_temp, "/gearname/%s/temp", ALIAS);
      sprintf(topic_humid, "/gearname/%s/humid", ALIAS);
      sprintf(topic_hic, "/gearname/%s/hic", ALIAS);
      sprintf(topic_mq7, "/gearname/%s/mq7", ALIAS);
      sprintf(topic_mq2, "/gearname/%s/mq2", ALIAS);
      sprintf(topic_dust, "/gearname/%s/dust", ALIAS);

      microgear.publish(topic_temp, String(t), true);
      microgear.publish(topic_humid, String(h));
      microgear.publish(topic_hic, String(hic));
      microgear.publish(topic_mq7, String(mq7));
      microgear.publish(topic_mq2, String(mq2));
      microgear.publish(topic_dust, String(concentration));
    
      timer = 0;
    } else timer += 100;
  } else {
    //Serial.println("connection lost, reconnect...");
    if (timer >= 5000) {
      microgear.connect(APPID);
      timer = 0;
    } else timer += 100;
  }
}

