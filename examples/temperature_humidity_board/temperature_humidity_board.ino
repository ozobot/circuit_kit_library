/// Please install Seeed_Arduino_AHT20 library for this example

#include "drvkit.h"

#include "Wire.h"

#include "AHT20.h"

using namespace ozobot::drvkit;

AHT20 AHT;

void setup() {
  Serial.begin();
  while(!Serial) {}; /// Wait until Serial is ready

  Wire.begin();

  /// We expect temperature sensor in SensorTop1 slot
  CommunicateWith(SensorTop1);
  AHT.begin();
}

void loop() {
  float humi, temp;
    
  int ret = AHT.getSensor(&humi, &temp);
  
  if(ret) {
    Serial.printf("temperature: %.1f°C humidity: %.1f%%\r\n", temp, humi*100);
  }
  else {
    Serial.println("Get data from the sensor failed.");
  }
  
  delay(100);
}
