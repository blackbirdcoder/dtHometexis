#include "light.hpp"

ClientDigitalTwin::Light::Light(std::string name, std::string type,
                                std::string unit, float value, Vector3 position,
                                float angle, ClientDigitalTwin::Option options,
                                ClientDigitalTwin::Mode mode)
    : Sensor(name, type, unit, value, position, angle, options, mode) {
  this->isOn = static_cast<bool>(value);
  this->oldStateOn = this->isOn;
  this->warning = {5.0f, 90.0f};
  this->min = 1.0f;
  this->max = 100.0f;
  this->parsingOption();
}

void ClientDigitalTwin::Light::ShowWindow(const Camera3D &camera) {
  Sensor::ShowWindow(camera);

  if (this->mode != this->oldMode) {
    this->isOn = static_cast<bool>(value);
    this->parsingOption();
    this->oldMode = this->mode;
  }

  // Current value notifications
  std::string lightText = this->type + ": " + (this->isOn ? "on" : "off");
  GuiLabel(
      {this->windowRect.x + 5.0f, this->windowRect.y + 25.0f, 200.0f, 20.0f},
      lightText.c_str());
  //-----

  bool isControl = this->mode == ClientDigitalTwin::Mode::CONTROL;

  // Value bright
  if (!this->isOn) {
    GuiDisable();
  }
  GuiSlider(
      {this->windowRect.x + 49.0f, this->windowRect.y + 55.0f, 100.0f, 15.0f},
      "Bright ", TextFormat("%i%%", static_cast<int>(this->bright.value)),
      &this->bright.value, this->bright.min, this->bright.max);
  if (this->bright.value != this->bright.oldValue && isControl) {
    this->makeOption();
    this->isChangeOption = true;
  }

  this->bright.oldValue = this->bright.value;

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
  if (this->isOn != this->oldStateOn && isControl) {
    this->makeValue();
    this->isChangeValue = true;
  } else {
    //...
  }

  this->oldStateOn = this->isOn;
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

void ClientDigitalTwin::Light::parsingOption() {
  try {
    this->bright = {this->options.at("bright"), this->options.at("bright"),
                    this->min, this->max};
  } catch (...) {
    this->bright = {20.0f, 20.0f, this->min, this->max};
  }
}

void ClientDigitalTwin::Light::makeOption() {
  this->sendOption["bright"] = this->bright.value;
}

void ClientDigitalTwin::Light::makeValue() {
  this->sendValue["value"] = static_cast<float>(this->isOn);
}