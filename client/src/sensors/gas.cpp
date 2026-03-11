#include "gas.hpp"

ClientDigitalTwin::Gas::Gas(std::string name, std::string type,
                            std::string unit, float value, Vector3 position,
                            float angle, ClientDigitalTwin::Option options,
                            ClientDigitalTwin::Mode mode)
    : Sensor(name, type, unit, value, position, angle, options, mode) {
  this->value = value;
  this->supply = {this->value, 0.0f, 100.0f};

  try {
    this->limit = this->options.at("limit");
  } catch (...) {
    this->limit = 5.0f;
  }
}

void ClientDigitalTwin::Gas::ShowWindow(const Camera3D &camera) {
  Sensor::ShowWindow(camera);

  // Current value notifications
  int value = static_cast<int>(this->supply.value);
  std::string openText = this->type + ": " + (value > 0 ? "yes" : "no");
  GuiLabel(
      {this->windowRect.x + 5.0f, this->windowRect.y + 25.0f, 200.0f, 20.0f},
      openText.c_str());

  std::string statusText = "Status: ";
  statusText += value > static_cast<int>(this->limit) ? "accident" : "normal";
  GuiLabel(
      {this->windowRect.x + 5.0f, this->windowRect.y + 55.0f, 200.0f, 20.0f},
      statusText.c_str());
  //-----

  // Let it smoke simulation
  bool isControl = this->mode == ClientDigitalTwin::Mode::CONTROL;

  if (isControl) {
    GuiDisable();
  }
  GuiSlider(
      {this->windowRect.x + 52.0f, this->windowRect.y + 85.0f, 100.0f, 15.0f},
      "Supply ", TextFormat("%i%%", static_cast<int>(this->supply.value)),
      &this->supply.value, this->supply.min, this->supply.max);
  if (isControl) {
    GuiEnable();
  }

  //-----

  // Show indicate
  if (this->supply.value >= this->limit) {
    this->indicateColor = RED;
  } else {
    this->indicateColor = GREEN;
  }
  //-----
}