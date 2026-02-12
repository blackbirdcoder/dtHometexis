#include "sensor.hpp"
#include <sstream>

ClientDigitalTwin::Sensor::Sensor(std::string name, std::string type,
                                  std::string unit, double value) {
  this->name = name;
  this->type = type;
  this->unit = unit;
  this->value = value;
}

std::string ClientDigitalTwin::Sensor::GetIndication() const {
  std::ostringstream str;

  if (unit != "bool") {
    str << type << ":" << value << unit;
  } else {
    str << type << ":" << (value ? "on" : "off");
  }

  return str.str();
}