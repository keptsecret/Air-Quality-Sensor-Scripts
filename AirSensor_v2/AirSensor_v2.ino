/* Adapted for using with netpie
 *  Status: work in progress...
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

//data var
char data[32];

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

/* If a new message arrives, do this */
void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
    Serial.print("Incoming message --> ");
    msg[msglen] = '\0';
    Serial.println((char *)msg);
}

void onFoundgear(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.print("Found new member --> ");
    for (int i=0; i<msglen; i++)
        Serial.print((char)msg[i]);
    Serial.println();  
}

void onLostgear(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.print("Lost member --> ");
    for (int i=0; i<msglen; i++)
        Serial.print((char)msg[i]);
    Serial.println();
}

/* When a microgear is connected, do this */
void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.println("Connected to NETPIE...");
    /* Set the alias of this microgear ALIAS */
    microgear.setName(ALIAS);
}

void setup() {

  //begin

  // Call onMsghandler() when new message arraives
  microgear.on(MESSAGE,onMsghandler);

  // Call onFoundgear() when new gear appear
  microgear.on(PRESENT,onFoundgear);

  // Call onLostgear() when some gear goes offline
  microgear.on(ABSENT,onLostgear);

  // Call onConnected() when NETPIE connection is established
  microgear.on(CONNECTED,onConnected);
    
  Serial.begin(115200);

  Serial.println("Air Quality Sensors test");
  
  //connect to wifi
  Serial.print("Connecting to WiFi... ");
  if (WiFi.begin(ssid, password)) {
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
  }

  /* Initialize with KEY, SECRET and also set the ALIAS here */
  microgear.init(KEY,SECRET,ALIAS);

  /* connect to NETPIE to a specific APPID */
  microgear.connect(APPID);

  //begin temperature sensor
  dht.begin();

}

void loop() {

  delay(60000); //every 1 minute

  if (microgear.connected()) {
    Serial.println("connected");
    //start json string
    String dataJson = "";

    microgear.loop();

    if (timer>=1000) {
      //read from DHT22
      float h = dht.readHumidity();
      //dataJson += String("humdity: ");
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
      Serial.print("reading mq7...");
      digitalWrite(mq7pin, HIGH);
      mq7_value = analogRead(analogpin);
      digitalWrite(mq7pin, LOW);
      Serial.println("done!");
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
      Serial.print("reading mq2...");
      digitalWrite(mq2pin, HIGH);
      mq2_value = analogRead(analogpin);
      digitalWrite(mq2pin, LOW);
      Serial.println("done!");
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
      microgear.publish("/airsensor", dataJson);
    } else timer += 100;
    
  } else {
      Serial.println("connection lost, reconnect...");
      if (timer >= 5000) {
          microgear.connect(APPID);
          timer = 0;
      }
      else timer += 100;
    }
}
