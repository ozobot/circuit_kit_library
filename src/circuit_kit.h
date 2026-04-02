#ifndef OZOBOT_SENSORS_H
#define OZOBOT_SENSORS_H

#include <string>
#include <memory>

#include "Arduino.h"
#include "sensor_description.h"

namespace ozobot::circuit_kit {

class BaseSensor {
public:
  BaseSensor(char const * name, uint8_t id) : name(name), id(id) {};
  char const * const name;
  uint8_t const id;
  static constexpr uint8_t const sharedInterrupt = D20;
};

class GenericSensor : public BaseSensor {
public:
  GenericSensor(char const * name, uint8_t id, uint8_t gpio, uint8_t adc)
      : BaseSensor(name,id),
        gpio(gpio),
        adc(adc)
  {};

  uint8_t const gpio;
  uint8_t const adc;
};

class DisplaySensor : public BaseSensor {
public:
  DisplaySensor(char const * name, uint8_t id, uint8_t gpio_0, uint8_t gpio_1, uint8_t gpio_2, uint8_t gpio_3)
      : BaseSensor(name, id),
        gpio_0(gpio_0),
        gpio_1(gpio_1),
        gpio_2(gpio_2),
        gpio_3(gpio_3)
  {};

  uint8_t const gpio_0;
  uint8_t const gpio_1;
  uint8_t const gpio_2;
  uint8_t const gpio_3;
};

class AnalogSensor : public BaseSensor {
public:
  AnalogSensor(char const * name, uint8_t id, uint8_t adc_0, uint8_t adc_1, uint8_t adc_2, uint8_t adc_3, uint8_t adc_4)
      : BaseSensor(name, id),
        adc_0(adc_0),
        adc_1(adc_1),
        adc_2(adc_2),
        adc_3(adc_3),
        adc_4(adc_4)
  {};

  uint8_t const adc_0;
  uint8_t const adc_1;
  uint8_t const adc_2;
  uint8_t const adc_3;
  uint8_t const adc_4;
};

void Init();
void CommunicateWith(const BaseSensor & sensor);
std::shared_ptr<SensorDescription> GetSensorDescription(BaseSensor const &sensor);

extern GenericSensor const SensorLeft;
extern GenericSensor const SensorFront;
extern GenericSensor const SensorRight;
extern GenericSensor const SensorTop1;
extern GenericSensor const SensorTop2;

extern DisplaySensor const HMI;

extern AnalogSensor const SensorLine;

extern BaseSensor const SensorBattery;

extern BaseSensor const * const SensorsAll[8];

}

#endif