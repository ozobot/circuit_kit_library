#include "circuit_kit.h"
#include "Wire.h"

using namespace ozobot::circuit_kit;

void setup() {
  Serial.begin();
  Wire.begin();

  ozobot::circuit_kit::Init();

  pinMode(BUTTON, INPUT);

  delay(500);
  for(BaseSensor const * sensor : AllSensors) {
    CommunicateWith(*sensor);
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
