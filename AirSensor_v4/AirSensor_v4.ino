/* Air Quality Sensor v4
 *  for using with homemade mysql server on RPi
 *  Work in progress
 */

//#include <MicroGear.h>
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
/*#define APPID   "AirSensorPrototype"
#define KEY     "3LyDlYWe4KhPnj2"
#define SECRET  "FKpkm0ZxvKPvDR8uQPRLE960a"
#define ALIAS   "esp8266"*/
const char* host     = "192.168.1.45";
const char* passcode = "22mar44";
const char* ssid     = "Dell_pc";
const char* password = "0853348548";

WiFiClient client;
int timer = 0;
//MicroGear microgear(client);

/*void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
    Serial.print("Incoming message -->");
    msg[msglen] = '\0';
    Serial.println((char *)msg);
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.println("Connected to NETPIE...");
    microgear.setAlias(ALIAS);
}*/

void setup() {
  //begin
  dht.begin();

  //microgear.on(MESSAGE, onMsghandler);
  //microgear.on(MESSAGE, onConnected);

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
  //microgear.init(KEY,SECRET,ALIAS);
  //microgear.connect(APPID);
}

void loop() {

  delay(60000); //every 1 minute

  //connect to host
  Serial.print("Connecting to ");
  Serial.println(host);
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("Connection failed");
    return;
  }
  
  //start json string
  String data = "/192.168.1.45/index.php?humid=";

  //read from DHT22
  float h = dht.readHumidity();
  //data += String("humdity: ");
  data += h;
  
  float t = dht.readTemperature();
  data += "&temp=";
  data += String(t);
  
  if (isnan(h) || isnan(t)){
    Serial.println("Failed to read from DHT sensor");
    return;
  }
  
  float hic = dht.computeHeatIndex(t, h, false);
  data += "&hic=";
  data += hic;

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
  data += "&mq7=";
  data += mq7;

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
  data += "&mq2=";
  data += mq2;
  
  //read from dust sensor
  duration = pulseIn(dustpin, LOW);
  lowpulseoccupancy = lowpulseoccupancy+duration;
  if ((millis()-starttime) > sampletime_ms) { //if the sample time == 30s
    ratio = lowpulseoccupancy/(sampletime_ms*10.0); // Integer percentage 0=>100
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
    lowpulseoccupancy = 0;
    starttime = millis();

    data += "&dust=";
    data += concentration;
  }

  data += "&pass=";
  data += passcode;    

  // This will send the request to the server
  Serial.print("Requesting URL: ");
  Serial.println(data);
  client.print(String("GET ") + data + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
/*
  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  */
  Serial.println();
  Serial.println("Closing connection");
}

