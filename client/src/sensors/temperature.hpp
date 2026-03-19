#pragma once
#include "sensor.hpp"
#include <cstdint>

namespace ClientDigitalTwin {
class Temperature : public Sensor {
public:
  Temperature(std::string name, std::string type, std::string unit, float value,
              Vector3 position, float angle, Option options, Mode mode);
  void ShowWindow(const Camera3D &camera) override;

private:
  struct RoomHeat {
    bool isHeating;
    bool oldState;
    char celsiusText[4];
    uint8_t min;
    uint8_t max;
  } roomHeat;
  float celsiusForSimulation;
  int celsius;
  int8_t indicationFire;
  int oldCelsius;
  bool isOptionParse;
  void parsingOption() override;
  void makeOption() override;
};
} // namespace ClientDigitalTwin