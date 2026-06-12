#include "sensor_description.h"

#include "Arduino.h"
#include "CRC32.h"

namespace ozobot::drvkit {

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
  if(sensorDescription->struct_version != 0) {
    std::ostringstream stream;
    stream << "Unsupported struct type " << (int) sensorDescription->struct_version << std::endl;
    return stream.str();
  }

  std::ostringstream stream;
  stream << "Board id " << sensorDescription->board.id << " revision " << sensorDescription->board.revision << std::endl;

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
      case DescriptionTypes::GPIO:
      {
        GPIODescription const * gpio = reinterpret_cast<GPIODescription const *>(description->data);
        stream << "GPIO id " << (int)gpio->id
               << " direction " << ToString(gpio->direction)
               << " pull " << ToString(gpio->pull)
               << " inverted " << (gpio->inverted ? "true" : "false");
        unsigned const desc_len = description->length - sizeof(GPIODescription);
        if(desc_len > 0) {
          stream << " description \"";
          stream.write(reinterpret_cast<char const *>(gpio->description), desc_len);
          stream << "\"";
        }
        stream << std::endl;
        break;
      }
      case DescriptionTypes::ADC:
      {
        ADCDescription const * adc = reinterpret_cast<ADCDescription const *>(description->data);
        stream << "ADC id " << (int)adc->id
               << " range_reference_valid " << (adc->range_reference_valid ? "true" : "false")
               << " range_reference " << (int)adc->range_reference;
        unsigned const desc_len = description->length - sizeof(ADCDescription);
        if(desc_len > 0) {
          stream << " description \"";
          stream.write(reinterpret_cast<char const *>(adc->description), desc_len);
          stream << "\"";
        }
        stream << std::endl;
        break;
      }
      case DescriptionTypes::Range:
      {
        RangeDescription const * range = reinterpret_cast<RangeDescription const *>(description->data);
        stream << "Range id " << (int)range->id
               << " raw [" << range->raw.min << ", " << range->raw.max << "]"
               << " output [" << range->output.min << ", " << range->output.max << "]"
               << " unit \"";
        stream.write(range->unit, RangeDescription::RANGE_UNIT_MAX_LENGTH);
        stream << "\"";
        unsigned const desc_len = description->length - sizeof(RangeDescription);
        if(desc_len > 0) {
          stream << " description \"";
          stream.write(reinterpret_cast<char const *>(range->description), desc_len);
          stream << "\"";
        }
        stream << std::endl;
        break;
      }
      case DescriptionTypes::I2C:
      {
        I2CDescription const * i2c = reinterpret_cast<I2CDescription const *>(description->data);
        stream << "I2C id " << (int)i2c->id
               << " address 0x" << std::hex << (int)i2c->address << std::dec
               << std::endl;
        break;
      }
      }

    description_ptr += description->length + sizeof(Description);
  }

  return stream.str();
}

}