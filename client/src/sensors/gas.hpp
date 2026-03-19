#pragma once
#include "sensor.hpp"

namespace ClientDigitalTwin {
class Gas : public Sensor {
public:
  Gas(std::string name, std::string type, std::string unit, float value,
      Vector3 position, float angle, Option options, Mode mode);
  void ShowWindow(const Camera3D &camera) override;

private:
  bool isOn;
  bool oldStateOn;
  float limit;
  struct {
    float value;
    float oldValue;
    float min;
    float max;
  } leak;
  void parsingOption() override;
  void makeValue() override;
};
} // namespace ClientDigitalTwin