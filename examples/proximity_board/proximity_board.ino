#include "drvkit.h"

using namespace ozobot::drvkit;

void setup() {
  Serial.begin();

  /// Set proximity board GPIO to output state 
  pinMode(SensorFront.gpio, OUTPUT);
  /// Turn emitting IR LED ON
  digitalWrite(SensorFront.gpio, HIGH);
}

void loop() {
  /// Read receiving LED output. It gives reflectance of an object in front of it.
  int const reflectance = analogRead(SensorFront.adc);
  /// Print measured value. Serial plotter can be used to make a graph from N values.
  Serial.printf("reflectance:%d\r\n", reflectance);
  /// Wait litle bit to not overwhelm an user with too fast update.
  delay(100);
}
