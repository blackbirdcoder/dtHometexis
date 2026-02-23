#pragma once
#include "sensor.hpp"

namespace ClientDigitalTwin {
class Smoke : public Sensor {
public:
  Smoke(std::string name, std::string type, std::string unit,
              double value, Vector3 position, float angle);
  void ShowWindow(const Camera3D &camera) override;

private:
    const char *test = "HELLO SMOKE";
};
} // namespace ClientDigitalTwin