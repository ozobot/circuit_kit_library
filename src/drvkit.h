#ifndef OZOBOT_SENSORS_H
#define OZOBOT_SENSORS_H

#include <string>
#include <memory>

#include "Arduino.h"
#include "Wire.h"
#include "sensor_description.h"

namespace ozobot::drvkit {

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

/**
 * Enable communication with multiple sensors using I2C multiplexer.
 * It should be called each time before communication with a sensor.
 * @param sensors Sensors to communicate with
 */
template <typename... Sensors>
void CommunicateWith(Sensors const & ... sensors) {
  static constexpr const uint8_t MULTIPLEXER_ADDRESS = 0b1110000;

  /// We need to convert the parameter pack to an array of pointers otherwise foreach loop will not work
  BaseSensor const * base_sensors[] = {&sensors...};

  /// Compound bitmask from individual sensors
  uint8_t bitmask = 0;
  for (BaseSensor const * sensor : base_sensors) {
    bitmask |= 1 << sensor->id;
  }

  Wire.beginTransmission(MULTIPLEXER_ADDRESS);
  Wire.write(bitmask);
  Wire.endTransmission();
}

/**
 * Bound a value in range <min, max>
 */
template <class Type>
inline Type BoundInRange(Type value, Type min, Type max) {
  return value < min  ? min
                      : value > max ? max
                                    : value;
}

class Motors {
public:
  enum Position {
    Left,
    Right,
    Both
  };

  Motors(Motors const &) = delete;
  void operator=(Motors const &) = delete;

  void begin();
  static Motors & GetInstance();
  void SetDuty(Position position, int duty);
  void Brake(Position position);
protected:
  Motors() {};

  /**
    * Set left motor duty.
    * @param duty Motor duty cycle in range <-100, 100>
    */
  void SetDutyLeft(int duty);
  /**
    * Set right motor duty.
    * @param duty Motor duty cycle in range <-100, 100>
    */
  void SetDutyRight(int duty);
private:
  static constexpr unsigned const ANALOG_WRITE_MAX = 255;
  static constexpr unsigned const PWM_FREQUENCY_HZ = 1000;

  static Motors motors_;
};

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

extern Motors & MotorsChassis;

inline void CommunicateWithAll() {
  CommunicateWith(SensorLeft, SensorFront, SensorRight, SensorTop1, SensorTop2, HMI, SensorLine, SensorBattery);
}

}

#endif