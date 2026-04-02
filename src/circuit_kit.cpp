#ifdef ARDUINO_OZOBOT_CIRCUIT_KIT

#include <sstream>

#include "circuit_kit.h"
#include "Wire.h"
#include "at24c02.h"

using namespace std;

namespace ozobot::circuit_kit {

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

BaseSensor const * const SensorsAll[8] = {
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

#endif