#include <SPI.h>
#include "SdFat.h"

SdFat SD;

File dataFile;

String filename = "air_quality_sensor.csv";

const int chipSelect = 10;

void writeHeader() {
  dataFile.print(F("humidity"));
  dataFile.print(F("temp."));
  dataFile.print(F("[dust conc.]"));
  dataFile.print(F("heat index"));
  dataFile.print(F("mq7"));
  dataFile.print(F("mq2"));
  dataFile.println();
  dataFile.close();
}

void setup() {
  Serial.begin(9600);

  if(!SD.begin(chipSelect)) {
    Serial.println("failed to initialize");
    return;
  }

  Serial.println("initialized");

  dataFile = SD.open(filename, FILE_WRITE);

  if (dataFile) {
    writeHeader();
  } else {
    Serial.println("error");
  }

}

void loop() {
  dataFile = SD.open(filename, FILE_WRITE);

  if (dataFile) {
    for (int i = 0; i < 6; i++) {
      dataFile.print(i+1);
    }
    dataFile.println();
  } else {
    Serial.print("error writing");
  }
}
