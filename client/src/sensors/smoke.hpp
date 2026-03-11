#pragma once
#include "sensor.hpp"

namespace ClientDigitalTwin {
class Smoke : public Sensor {
public:
  Smoke(std::string name, std::string type, std::string unit, float value,
        Vector3 position, float angle, Option options, Mode mode);
  void ShowWindow(const Camera3D &camera) override;

private:
  float value;
  float limit;
  struct {
    float value;
    float min;
    float max;
  } supply;
};
} // namespace ClientDigitalTwin