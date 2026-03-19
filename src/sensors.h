#ifndef OZOBOT_SENSORS_H
#define OZOBOT_SENSORS_H

#include <string>

#include "Arduino.h"

class BaseSensor {
public:
  BaseSensor(uint8_t id) : id(id) {};
  const uint8_t id;
  static constexpr const uint8_t sharedInterrupt = D20;
};

class GenericSensor : public BaseSensor {
public:
  GenericSensor(uint8_t id, uint8_t gpio, uint8_t adc) : BaseSensor(id), gpio(gpio), adc(adc) {};

  const uint8_t gpio;
  const uint8_t adc;
};

class DisplaySensor : public BaseSensor {
public:
  DisplaySensor(uint8_t id, uint8_t gpio_0, uint8_t gpio_1, uint8_t gpio_2, uint8_t gpio_3)
      : BaseSensor(id), gpio_0(gpio_0), gpio_1(gpio_1), gpio_2(gpio_2), gpio_3(gpio_3) {};

  const uint8_t gpio_0;
  const uint8_t gpio_1;
  const uint8_t gpio_2;
  const uint8_t gpio_3;
};

class AnalogSensor : public BaseSensor {
public:
  AnalogSensor(uint8_t id, uint8_t adc_0, uint8_t adc_1, uint8_t adc_2, uint8_t adc_3, uint8_t adc_4)
      : BaseSensor(id), adc_0(adc_0), adc_1(adc_1), adc_2(adc_2), adc_3(adc_3), adc_4(adc_4) {};

  const uint8_t adc_0 = A5;
  const uint8_t adc_1 = A6;
  const uint8_t adc_2 = A7;
  const uint8_t adc_3 = A8;
  const uint8_t adc_4 = A9;
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

  bool IsValid(unsigned length);
} __attribute__((packed));

static_assert(sizeof(SensorDescription) == 78, "Incorrect SensorDescription size should be 78B.");

std::string ToString(SensorDescription const * const sensorDescription);

void CommunicateWith(const BaseSensor & sensor);

extern const GenericSensor SensorLeft;
extern const GenericSensor SensorFront;
extern const GenericSensor SensorRight;
extern const GenericSensor SensorTop1;
extern const GenericSensor SensorTop2;

extern const DisplaySensor HMI;

extern const AnalogSensor LineSensor;

extern const BaseSensor BatterySensor;

#endif