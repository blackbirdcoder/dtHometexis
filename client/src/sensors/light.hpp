#pragma once
#include "sensor.hpp"

namespace ClientDigitalTwin {
class Light : public Sensor {
public:
  Light(std::string name, std::string type, std::string unit, float value,
        Vector3 position, float angle, Option options, Mode mode);
  void ShowWindow(const Camera3D &camera) override;

private:
  bool isOn;
  bool oldStateOn;
  float min;
  float max;
  struct {
    float value;
    float oldValue;
    float min;
    float max;
  } bright;
  struct {
    float min;
    float max;
  } warning;
  void parsingOption() override;
  void makeOption() override;
  void makeValue() override;
};
} // namespace ClientDigitalTwin