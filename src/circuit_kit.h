#ifndef OZOBOT_SENSORS_H
#define OZOBOT_SENSORS_H

#include <string>
#include <memory>

#include "Arduino.h"

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

enum class PinMode : uint8_t {
  Digital = 0,
  Analog = 1,
};

std::string ToString(PinMode const mode);

enum class Direction : uint8_t {
  None = 0,
  Input = 1,
  Output = 2,
  Bidirectional = 3,
};

std::string ToString(Direction const direction);

enum class Pull : uint8_t {
  None = 0,
  Up = 1,
  Down = 2,
};

std::string ToString(Pull const pull);

struct GPIODescription {
  static constexpr const unsigned RANGE_UNIT_MAX_LENGTH = 8;

  union {
    struct {
      PinMode mode : 1;
      Direction direction : 2;
      Pull pull : 2;
      uint8_t inverted : 1;
      uint8_t rangeValid : 1;
      uint8_t reserved : 1;
    } __attribute__((packed));
    uint8_t byte;
  } __attribute__((packed));

  struct {
    struct {
      uint32_t min;
      uint32_t max;
    } raw;
    struct {
      int32_t min;
      int32_t max;
    } output;
    char unit[RANGE_UNIT_MAX_LENGTH];
  } __attribute__((packed)) range;

  uint8_t reserved_2[7];
} __attribute__((packed));

static_assert(sizeof(GPIODescription) == 32, "Incorrect GPIODescription size should be 32B.");

enum DescriptionTypes {
  CRC32 = 0,
  BoardName = 1,
  String = 2,
};

struct Description {
  uint8_t length;
  uint8_t type;
  uint8_t data[];
} __attribute__((packed));

struct SensorDescription {
  uint8_t id;
  uint8_t reserved[3];
  struct {
    uint32_t id;
    uint32_t revision;
  } __attribute__((packed)) board;
  GPIODescription gpio[2];
  struct {
    uint16_t address;
  } __attribute__((packed)) i2c;
  uint8_t descriptions[];

  bool IsValid() const;
  unsigned Length() const;

  static constexpr unsigned const MAX_LENGTH = 256;
} __attribute__((packed));

static_assert(sizeof(SensorDescription) == 78, "Incorrect SensorDescription size should be 78B.");

std::string ToString(SensorDescription const * const sensorDescription);

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