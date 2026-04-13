#pragma once
#include "sensor.hpp"

namespace ClientDigitalTwin {
class Door : public Sensor {
public:
  Door(std::string name, std::string type, std::string unit, float value,
       Vector3 position, float angle, Option options, Mode mode);
  void ShowWindow(const Camera3D &camera) override;

private:
  bool oldStateOpen;
  bool isAlarm;
  bool oldStateAlarm;
  void parsingOption() override;
  void makeOption() override;
  void makeValue() override;
};
} // namespace ClientDigitalTwin