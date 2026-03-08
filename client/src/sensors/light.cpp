#include "light.hpp"

ClientDigitalTwin::Light::Light(std::string name, std::string type,
                                std::string unit, float value, Vector3 position,
                                float angle, ClientDigitalTwin::Option options,
                                ClientDigitalTwin::Mode mode)
    : Sensor(name, type, unit, value, position, angle, options, mode) {
  this->isOn = static_cast<bool>(value);
  this->warning = {5.0f, 90.0f};

  float min = 1.0f;
  float max = 100.0f;
  try {
    this->bright = {this->options.at("bright"), min, max};
  } catch (...) {
    this->bright = {20.0f, min, max};
  }
}

void ClientDigitalTwin::Light::ShowWindow(const Camera3D &camera) {
  Sensor::ShowWindow(camera);

  // Current value notifications
  std::string lightText = this->type + ": " + (this->isOn ? "on" : "off");
  GuiLabel(
      {this->windowRect.x + 5.0f, this->windowRect.y + 25.0f, 200.0f, 20.0f},
      lightText.c_str());
  //-----

  // Value bright
  if (!this->isOn) {
    GuiDisable();
  }
  GuiSlider(
      {this->windowRect.x + 49.0f, this->windowRect.y + 55.0f, 100.0f, 15.0f},
      "Bright ", TextFormat("%i%%", static_cast<int>(this->bright.value)),
      &this->bright.value, this->bright.min, this->bright.max);
  if (!this->isOn) {
    GuiEnable();
  }
  //-----

  // Toggle light
  if (GuiButton({this->windowRect.x + 143.0f, this->windowRect.y + 102.0f,
                 100.0f, 30.0f},
                "Toggle")) {
    this->isOn = !isOn;
  }
  if (this->isOn) {
    //...
  } else {
    //...
  }
  //-----

  // Indicate
  if (this->bright.value > this->warning.max ||
      this->bright.value < this->warning.min) {
    this->indicateColor = RED;
  } else {
    this->indicateColor = GREEN;
  }

  if (!this->isOn) {
    this->indicateColor = GREEN;
  }
  //-----
}