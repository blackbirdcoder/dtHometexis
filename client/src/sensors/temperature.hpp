#pragma once
#include "sensor.hpp"

namespace ClientDigitalTwin {
class Temperature : public Sensor {
public:
  Temperature(std::string name, std::string type, std::string unit,
              double value, Vector3 position, float angle);
  void ShowWindow(const Camera3D &camera) override;

private:
    const char *test = "HELLO TEMP";
};
} // namespace ClientDigitalTwin