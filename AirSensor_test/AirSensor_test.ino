//#include <SD.h>

#include <SPI.h>

#include "SdFat.h"

#include "DHT.h"

#define DHTPIN 2

#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

SdFat SD;

File dataFile;

int pin = 8;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 30000;//sampe 30s ;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;

const int chipSelect = 10; //change accordingly

/*
void writeHeader() {
  dataFile = SD.open("air_quality_log.txt", FILE_WRITE);

  dataFile.println("Air Quality Log");
  dataFile.println("humidity, temperature, [dust conc.], heat index, mq7, mq2");
  dataFile.println(" ");
}
*/

void setup() {
  //Serial.begin(9600);
  //Serial.println("Air Quality Sensors test");
  pinMode(8,INPUT);
  starttime = millis();
  
  dht.begin();
  
  //Serial.print("Initializing SD card...");
  pinMode(chipSelect, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  if (!SD.begin(chipSelect)) {
    //Serial.println("Card failed or not present");
    return; //stop
  }

  //Serial.println("card initialized");

  dataFile = SD.open("air_quality_log.txt", FILE_WRITE);

  if (dataFile) {
    dataFile.println("Air Quality Log");
    dataFile.println("humidity, temperature, [dust conc.], heat index, mq7, mq2");
    dataFile.println(" ");
    dataFile.close();
  } else {
    //Serial.println("failed to open file");
    return;
  }
}

void loop() {

  String dataString = "";

  float h = dht.readHumidity();
  dataString += String(h); //append to string
  dataString += ", ";
  
  float t = dht.readTemperature();
  dataString += String(t); //append to string
  dataString += ", ";
  
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

      dataString += String(concentration); //append to string
      dataString += ", ";
    }

  if (isnan(h) || isnan(t)) {
    //Serial.println("Failed to read from DHT sensor");
    return;
  }

  float hic = dht.computeHeatIndex(t, h, false);
  dataString += String(hic); //append to string
  dataString += ", ";

  mq7Value = analogRead(A0);
  mq7Volt = (float)mq7Value / 1024 * 5.0;
  mq7_gas = (5.0-mq7Volt)/mq7Volt;
  mq7_ratio = mq7_gas/0.18;
  dataString += String(mq7_ratio); //append to string
  dataString += ", ";

  mq2Value = analogRead(A1);
  mq2Volt = (float)mq2Value / 1024 * 5.0;
  mq2_gas = (5.0-mq2Volt)/mq2Volt;
  mq2_ratio = mq2_gas/0.40;
  dataString += String(mq2_ratio); //append to string
  dataString += ", ";

  
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print("%\t");
  Serial.print("Temp.: ");
  Serial.print(t);
  Serial.print("*C\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.println("*C");
  
  Serial.print("MQ2 Volt: ");
  Serial.print(mq2Volt);
  Serial.print("V\t");
  Serial.print("MQ2 RS/R0: ");
  Serial.println(mq2_ratio);
  
  Serial.print("MQ7 Volt: ");
  Serial.print(mq7Volt);
  Serial.print("V\t");
  Serial.print("MQ7 RS/R0: ");
  Serial.print(mq7_ratio);
  Serial.println("\t");
  Serial.println(" ");
  
  /*
  //open and write to file
  dataFile = SD.open("air_quality_log.txt", FILE_WRITE);

  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    //digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    //digitalWrite(LED_BUILTIN, LOW);
  } else {
    //Serial.println("error opening log file");
    return;                                                                               
  }
  */

  delay(60000);
}
