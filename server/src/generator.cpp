#include "generator.hpp"
#include <random>

ServerDigitalTwin::Generator::Generator() {}

nlohmann::json ServerDigitalTwin::Generator::Generate(const std::string &room) {
  nlohmann::json sensor = nlohmann::json::array();
  if (room == "kitchen") {
    sensor.push_back({
        {"name", "kitchen_temp_1"},
        {"type", "temperature"},
        {"unit", "C"},
        {"value", randomValue(-50.0, 50.0)},
    });
    sensor.push_back({
        {"name", "kitchen_hum_1"},
        {"type", "humidity"},
        {"unit", "%"},
        {"value", randomValue(0.0, 100.0)},
    });
    sensor.push_back({
        {"name", "kitchen_gas_1"},
        {"type", "gas"},
        {"unit", "bool"},
        {"value", 0.0},
    });
    sensor.push_back({
        {"name", "kitchen_smoke_1"},
        {"type", "smoke"},
        {"unit", "bool"},
        {"value", 0.0},
    });
    sensor.push_back({
        {"name", "kitchen_light_1"},
        {"type", "light"},
        {"unit", "bool"},
        {"value", 1.0},
    });
    sensor.push_back({
        {"name", "kitchen_motion_1"},
        {"type", "motion"},
        {"unit", "bool"},
        {"value", 0.0},
    });
  }
  return sensor;
}

double ServerDigitalTwin::Generator::randomValue(double min, double max) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> dist(min, max);
  return dist(gen);
}