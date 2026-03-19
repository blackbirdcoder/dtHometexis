#pragma once
#include "sensor.hpp"

namespace ClientDigitalTwin {
class Humidity : public Sensor {
public:
  Humidity(std::string name, std::string type, std::string unit, float value,
           Vector3 position, float angle, Option option, Mode mode);
  void ShowWindow(const Camera3D &camera) override;

private:
  struct Humidifier {
    bool humify;
    bool oldState;
    uint8_t min;
    uint8_t max;
    char percentText[4];
  } humidifier;
  uint8_t indicationLeak;
  float humidityForSimulation;
  uint8_t humidity;
  uint8_t oldHumidity;
  void parsingOption() override;
  void makeOption() override;
};
} // namespace ClientDigitalTwin