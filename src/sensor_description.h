#ifndef OZOBOT_SENSOR_DESCRIPTION_H
#define OZOBOT_SENSOR_DESCRIPTION_H

#include <cstdint>
#include <string>
#include <sstream>

namespace ozobot::drvkit {

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
  union {
    struct {
      uint8_t id : 3;
      Direction direction : 2;
      Pull pull : 2;
      uint8_t inverted : 1;
    } __attribute__((packed));
    uint8_t byte;
  } __attribute__((packed));
} __attribute__((packed));

static_assert(sizeof(GPIODescription) == 1, "Incorrect GPIODescription size should be 32B.");

struct ADCDescription {
  static constexpr const unsigned RANGE_UNIT_MAX_LENGTH = 8;

  union {
    struct {
      uint8_t id : 3;
      Direction direction : 2;
      Pull pull : 2;
      uint8_t inverted : 1;
    } __attribute__((packed));
    uint8_t low;
  } __attribute__((packed));

  union {
    struct {
      uint8_t rangeValid : 1;
      uint8_t reserved : 7;
    } __attribute__((packed));
    uint8_t high;
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
} __attribute__((packed));

static_assert(sizeof(ADCDescription) == 26, "Incorrect ADCDescription size should be 26B.");

enum DescriptionTypes {
  CRC32 = 0,
  BoardName = 1,
  String = 2,
  GPIO = 3,
  ADC = 4,
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
    uint16_t revision;
  } __attribute__((packed)) board;
  uint32_t features;
  struct {
    uint16_t address;
  } __attribute__((packed)) i2c;
  uint8_t descriptions[];

  bool IsValid() const;
  unsigned Length() const;

  static constexpr unsigned const MAX_LENGTH = 256;
} __attribute__((packed));

static_assert(sizeof(SensorDescription) == 16, "Incorrect SensorDescription size should be 16B.");

std::string ToString(SensorDescription const * const sensorDescription);

}

#endif
