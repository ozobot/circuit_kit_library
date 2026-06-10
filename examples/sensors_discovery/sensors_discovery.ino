#include "drvkit.h"
#include "Wire.h"

using namespace ozobot::drvkit;

void setup() {
  Serial.begin();
  Wire.begin();

  pinMode(BUTTON, INPUT);

  delay(500);
  for(BaseSensor const * sensor : SensorsAll) {
    Serial.printf("%s: ", sensor->name);

    auto description = GetSensorDescription(*sensor);
    if(!description) {
      Serial.printf("not present\r\n");
    } else {
      Serial.printf("\r\n%s", ToString(description.get()).c_str());
    }
  }
}

void loop() {
}
