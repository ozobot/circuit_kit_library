#include "drvkit.h"

using namespace ozobot::drvkit;

void setup() {
  Serial.begin(9600); 

  pinMode(BUTTON, INPUT);
}

static unsigned offsets[5] = {0};
static constexpr unsigned const THRESHOLD = 100;
static constexpr unsigned const MAX_POWER = 70;

void loop() {
  /// Take white paper offsets 
  {
    while(digitalRead(BUTTON) != LOW);

    unsigned * offset = offsets;
    for(auto const pin : {SensorLine.adc_0, SensorLine.adc_1, SensorLine.adc_2, SensorLine.adc_3, SensorLine.adc_4}) {
      *offset = analogRead(pin);
      offset++;
    }

    Serial.printf("White offsets [%d,%d,%d,%d,%d]\r\n", offsets[0], offsets[1], offsets[2], offsets[3], offsets[4]);
  }

  delay(500);
  while(digitalRead(BUTTON) != LOW);

  while(true) {
    unsigned lineSensors[5];
    unsigned * sensor = lineSensors;
    unsigned const * offset = offsets;
    for(auto const pin : {SensorLine.adc_0, SensorLine.adc_1, SensorLine.adc_2, SensorLine.adc_3, SensorLine.adc_4}) {
      *sensor = BoundInRange<int>(analogRead(pin) - *offset, 0, 4096);
      sensor++;
      offset++;
    }

    Serial.printf("%4d %4d %4d %4d %4d\r\n", lineSensors[0], lineSensors[1], lineSensors[2], lineSensors[3], lineSensors[4]);

    if(lineSensors[2] > THRESHOLD) {
      ChassisMotors.SetDuty(Motors::Both, MAX_POWER);
    } else if (lineSensors[1] > THRESHOLD) {
      ChassisMotors.SetDuty(Motors::Left, MAX_POWER / 2);
      ChassisMotors.SetDuty(Motors::Right, MAX_POWER);
    } else if (lineSensors[3] > THRESHOLD) {
      ChassisMotors.SetDuty(Motors::Left, MAX_POWER);
      ChassisMotors.SetDuty(Motors::Right, MAX_POWER / 2);
    } else if (lineSensors[0] > THRESHOLD) {
      ChassisMotors.SetDuty(Motors::Left, 0);
      ChassisMotors.SetDuty(Motors::Right, MAX_POWER);
    } else if (lineSensors[4] > THRESHOLD) {
      ChassisMotors.SetDuty(Motors::Left, MAX_POWER);
      ChassisMotors.SetDuty(Motors::Right, 0);
    }else {
      ChassisMotors.Brake(Motors::Both);
    }

    delay(10);
  }
}
