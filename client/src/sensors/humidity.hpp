#pragma once
#include "sensor.hpp"

namespace ClientDigitalTwin {
class Humidity : public Sensor {
public:
  Humidity(std::string name, std::string type, std::string unit,
              double value, Vector3 position, float angle);
  void ShowWindow(const Camera3D &camera) override;

private:
    const char *test = "HELLO HUM";
};
} // namespace ClientDigitalTwin