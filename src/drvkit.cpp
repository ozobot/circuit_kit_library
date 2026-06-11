#ifdef ARDUINO_OZOBOT_DRVKIT

#include <sstream>

#include "drvkit.h"
#include "Wire.h"
#include "at24c02.h"

using namespace std;

namespace ozobot::drvkit {

shared_ptr<SensorDescription> GetSensorDescription(BaseSensor const &sensor) {
  CommunicateWith(sensor);
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

void Motors::SetDuty(Side const side, int const duty) {
  int const power = BoundInRange((duty * MAX_ANALOG_WRITE_VALUE) / 100, -MAX_ANALOG_WRITE_VALUE, MAX_ANALOG_WRITE_VALUE);

  switch(side) {
    case Left:
    {
      if(power >= 0) {
        analogWrite(MOTOR_L_IN1, power);
        analogWrite(MOTOR_L_IN2, 0);
      } else {
        analogWrite(MOTOR_L_IN1, 0);
        analogWrite(MOTOR_L_IN2, -power);
      }
    }
      break;
    case Right:
    {
      if(power >= 0) {
        analogWrite(MOTOR_R_IN1, power);
        analogWrite(MOTOR_R_IN2, 0);
      } else {
        analogWrite(MOTOR_R_IN1, 0);
        analogWrite(MOTOR_R_IN2, -power);
      }
    }
      break;
    case Both:
    {
      SetDuty(Left, duty);
      SetDuty(Right, duty);
    }
      break;
  }
}

void Motors::Brake(Side const side) {
  switch(side) {
    case Left:
    {
      analogWrite(MOTOR_L_IN1, MAX_ANALOG_WRITE_VALUE);
      analogWrite(MOTOR_L_IN2, MAX_ANALOG_WRITE_VALUE);
    }
      break;
    case Right:
    {
      analogWrite(MOTOR_R_IN1, MAX_ANALOG_WRITE_VALUE);
      analogWrite(MOTOR_R_IN2, MAX_ANALOG_WRITE_VALUE);
    }
      break;
    case Both:
    {
      Brake(Left);
      Brake(Right);
    }
      break;
  }
}

Motors & Motors::GetInstance() {
  return instance_;
}

Motors Motors::instance_;

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

Motors & ChassisMotors = Motors::GetInstance();

}

#endif