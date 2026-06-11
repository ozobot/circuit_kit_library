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
    /**
     * @brief Which motor(s) to target.
     */
    enum Side {
      Left,   ///< Left motor only
      Right,  ///< Right motor only
      Both,   ///< Both motors simultaneously
    };

    /**
     * @brief Set motor speed and direction as a duty cycle percentage.
     *
     * Maps the duty percentage (range `-100` to `+100`) onto the hardware PWM
     * range (`-MAX_ANALOG_WRITE_VALUE` to `+MAX_ANALOG_WRITE_VALUE`).  Positive
     * values drive the motor forward; negative values reverse it.
     *
     * @param side  Which motor(s) to control (`Left`, `Right`, or `Both`).
     * @param duty  Duty cycle in percent: `-100` (full reverse) … `+100` (full
     *              forward).  A value of `0` coasts the motor (neither pin
     *              driven).  Values outside the range are clamped.
     */
    void SetDuty(Side side, int duty);

    /**
     * @brief Actively brake the selected motor(s).
     *
     * @param side  Which motor(s) to brake (`Left`, `Right`, or `Both`).
     */
    void Brake(Side side);

    /**
     * @brief Obtain the singleton instance of the motor controller.
     *
     * @return Reference to the single `Motors` object.
     */
    static Motors & GetInstance();
protected:
  /** Maximum PWM value understood by analogWrite() on this platform. */
  static constexpr signed const MAX_ANALOG_WRITE_VALUE = 255;

  Motors() = default;
  Motors(Motors const &) = delete;
  Motors const & operator=(Motors const &) = delete;

  static Motors instance_;
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

extern Motors & ChassisMotors;

inline void CommunicateWithAll() {
  CommunicateWith(SensorLeft, SensorFront, SensorRight, SensorTop1, SensorTop2, HMI, SensorLine, SensorBattery);
}

}

#endif