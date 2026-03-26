#include <sstream>

#include "circuit_kit.h"
#include "Wire.h"
#include "CRC32.h"
#include "at24c02.h"

using namespace std;

namespace ozobot::circuit_kit {

std::string ToString(PinMode const mode) {
  switch(mode) {
  case PinMode::Digital:
    return "digital";
  case PinMode::Analog:
    return "analog";
  }

  return "unknown";
}

std::string ToString(Direction const direction) {
  switch(direction) {
  case Direction::None:
    return "none";
  case Direction::Input:
    return "input";
  case Direction::Output:
    return "output";
  case Direction::Bidirectional:
    return "bidirectional";
  }

  return "unknown";
}

std::string ToString(Pull const pull) {
  switch(pull) {
  case Pull::None:
    return "none";
  case Pull::Up:
    return "up";
  case Pull::Down:
    return "down";
  }

  return "unknown";
}

bool SensorDescription::IsValid() const {
  uint8_t const * description_ptr = descriptions;
  while(description_ptr < (reinterpret_cast<uint8_t const *>(this) + MAX_LENGTH)) {
    Description const * const description = reinterpret_cast<Description const *>(description_ptr);

    if(description->type == DescriptionTypes::CRC32) {
      uint32_t CRC = 0;
      memcpy(&CRC, description->data, sizeof(CRC));

      unsigned const binaryLength = description_ptr - reinterpret_cast<uint8_t const *>(this);
      uint32_t computedCRC = CRC32::calculate(reinterpret_cast<uint8_t const *>(this), binaryLength);
      return CRC == computedCRC;
    }
    description_ptr += description->length + sizeof(Description);
  }

  return false;
}

unsigned SensorDescription::Length() const {
  uint8_t const * description_ptr = descriptions;
  while(description_ptr < (reinterpret_cast<uint8_t const *>(this) + MAX_LENGTH)) {
    Description const * const description = reinterpret_cast<Description const *>(description_ptr);

    if(description->type == DescriptionTypes::CRC32) {
      unsigned binaryLength = (description_ptr - reinterpret_cast<uint8_t const *>(this));
      return description->length + sizeof(Description) + binaryLength;
    }
    description_ptr += description->length + sizeof(Description);
  }

  return 0;
}

std::string ToString(SensorDescription const * const sensorDescription) {
  if(sensorDescription->id != 0) {
    std::ostringstream stream;
    stream << "Unsupported struct type " << (int) sensorDescription->id << std::endl;
    return stream.str();
  }

  std::ostringstream stream;
  stream << "Board id " << sensorDescription->board.id << " revision " << sensorDescription->board.revision << std::endl;

  for(unsigned index = 0; index < 2; index++) {
    stream << "GPIO " << index << ":" << std::endl;
    stream << "\t mode - " << ToString(sensorDescription->gpio[index].mode) << std::endl;
    stream << "\t direction - " << ToString(sensorDescription->gpio[index].direction) << std::endl;
    stream << "\t pull - " << ToString(sensorDescription->gpio[index].pull) << std::endl;
    stream << "\t inverted - " << (sensorDescription->gpio[index].inverted ? "inverted" : "not inverted") << std::endl;
    if(!sensorDescription->gpio[index].rangeValid) {
      stream << "\t range invalid" << std::endl;
    } else {
      stream << "\t range: " << std::endl;
      stream << "\t\t: raw - min " << sensorDescription->gpio[index].range.raw.min << " max " << sensorDescription->gpio[index].range.raw.max << std::endl;
      stream << "\t\t: output - min " << sensorDescription->gpio[index].range.output.min << " max " << sensorDescription->gpio[index].range.output.max << std::endl;
      stream << "\t\t: unit - " << sensorDescription->gpio[index].range.unit << std::endl;
    }
  }

  bool foundCRC = false;
  uint8_t const * description_ptr = sensorDescription->descriptions;
  while(!foundCRC) {
    Description const * const description = reinterpret_cast<Description const *>(description_ptr);

    switch(description->type) {
    case DescriptionTypes::CRC32:
    {
      uint32_t CRC = 0;
      memcpy(&CRC, description->data, sizeof(CRC));
      stream << "CRC: 0x"<< std::hex << CRC << std::endl;
      foundCRC = true;
      break;
    }
    case DescriptionTypes::String:
      stream << "String: ";
      stream.write(reinterpret_cast<char const *>(description->data), description->length);
      stream << std::endl;
      break;
    case DescriptionTypes::BoardName:
      stream << "Board name: ";
      stream.write(reinterpret_cast<char const *>(description->data), description->length);
      stream << std::endl;
      break;
    }

    description_ptr += description->length + sizeof(Description);
  }

  return stream.str();
}

void Init() {
  /// Pull I2C multiplexer reset high to allow communication via it
  pinMode(D5, OUTPUT);
  digitalWrite(D5, HIGH);
}

void CommunicateWith(BaseSensor const & sensor) {
  static constexpr const uint8_t MULTIPLEXER_ADDRESS = 0b1110000;
  Wire.beginTransmission(MULTIPLEXER_ADDRESS);
  Wire.write(1 << sensor.id);
  Wire.endTransmission();
}

shared_ptr<SensorDescription> GetSensorDescription(BaseSensor const &sensor) {
  AT24C02 eprom(AT24C_ADDRESS_0);

  uint8_t buffer[256];
  SensorDescription const * const sensorDescription = reinterpret_cast<SensorDescription const *>(buffer);
  unsigned const read = eprom.readBuffer(0, buffer, sizeof(buffer));
  if(read != sizeof(buffer) || !sensorDescription->IsValid()) {
    return {};
  }

  unsigned const length = sensorDescription->Length();
  uint8_t * memory = new uint8_t[length];
  if(memory == nullptr) {
    return {};
  }

  memcpy(memory, buffer, length);
  return shared_ptr<SensorDescription>(
    reinterpret_cast<SensorDescription *>(memory),
    [&](auto pointer) { delete [] memory; }
  );
}

GenericSensor const SensorLeft("sensor left", 0, D0, A0);
GenericSensor const SensorFront("sensor front", 1, D1, A1);
GenericSensor const SensorRight("sensor right", 2, D2, A2);
GenericSensor const SensorTop1("sensor top 1", 5, D4, A4);
GenericSensor const SensorTop2("sensor top 2", 4, D3, A3);

DisplaySensor const HMI("human-machine interface", 3, D6, D7, D8, D9);

AnalogSensor const SensorLine("sensor line", 6, A5, A6, A7, A8, A9);

BaseSensor const SensorBattery("sensor battery", 7);

BaseSensor const * const AllSensors[8] = {
    &SensorLeft,
    &SensorFront,
    &SensorRight,
    &SensorTop1,
    &SensorTop2,
    &HMI,
    &SensorLine,
    &SensorBattery,
};

}