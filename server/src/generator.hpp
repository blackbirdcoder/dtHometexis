#pragma once
#include <nlohmann/json.hpp>
#include <string>


namespace ServerDigitalTwin {
class Generator {
public:
  Generator();
  nlohmann::json Generate(const std::string &room);

private:
  double randomValue(double min, double max);
};
} // namespace ServerDigitalTwin