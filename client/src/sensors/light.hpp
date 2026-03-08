#pragma once
#include "sensor.hpp"

namespace ClientDigitalTwin {
class Light : public Sensor {
public:
  Light(std::string name, std::string type, std::string unit,
              float value, Vector3 position, float angle, Option options, Mode mode);
  void ShowWindow(const Camera3D &camera) override;

private:
  bool isOn;
  struct {
    float value;
    float min;
    float max;
  }bright;
  struct {
    float min;
    float max;
  } warning;
};
} // namespace ClientDigitalTwin