#pragma once
#include <string>

namespace ClientDigitalTwin {
class Sensor {
public:
  Sensor(std::string name, std::string type, std::string unit, double value);
  std::string GetIndication() const;

private:
  std::string name;
  std::string type;
  std::string unit;
  double value;
};
} // namespace ClientDigitalTwin