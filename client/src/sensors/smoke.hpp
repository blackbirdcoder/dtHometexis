#pragma once
#include "sensor.hpp"

namespace ClientDigitalTwin {
class Smoke : public Sensor {
public:
  Smoke(std::string name, std::string type, std::string unit, float value,
        Vector3 position, float angle, Option options, Mode mode);
  void ShowWindow(const Camera3D &camera) override;

private:
  bool isSmoke;
};
} // namespace ClientDigitalTwin