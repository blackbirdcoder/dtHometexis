#include "gas.hpp"

ClientDigitalTwin::Gas::Gas(std::string name, std::string type,
                            std::string unit, float value, Vector3 position,
                            float angle, ClientDigitalTwin::Option options,
                            ClientDigitalTwin::Mode mode)
    : Sensor(name, type, unit, value, position, angle, options, mode) {
  this->isOn = static_cast<bool>(value);
  this->parsingOption();
}

void ClientDigitalTwin::Gas::ShowWindow(const Camera3D &camera) {
  Sensor::ShowWindow(camera);

  if (this->mode != this->oldMode) {
    this->isOn = static_cast<bool>(value);
    this->parsingOption();
    this->oldMode = this->mode;
  }

  std::string openText = this->type + ": " + (this->isOn ? "on" : "off");
  GuiLabel(
      {this->windowRect.x + 5.0f, this->windowRect.y + 25.0f, 200.0f, 20.0f},
      openText.c_str());

  std::string statusText = "Status: ";
  statusText += this->leak.value >= this->limit ? "accident" : "normal";
  GuiLabel(
      {this->windowRect.x + 5.0f, this->windowRect.y + 55.0f, 200.0f, 20.0f},
      statusText.c_str());

  // Let it gas simulation
  bool isControl = this->mode == ClientDigitalTwin::Mode::CONTROL;

  if (isControl) {
    GuiDisable();
  }
  GuiSlider(
      {this->windowRect.x + 7.0f, this->windowRect.y + 85.0f, 100.0f, 15.0f},
      nullptr, TextFormat("%i%%", static_cast<int>(this->leak.value)),
      &this->leak.value, this->leak.min, this->leak.max);
  if (isControl) {
    GuiEnable();
  }

  // Toggle gas
  std::string btnText = this->isOn ? "Off" : "On";
  if (GuiButton({this->windowRect.x + 143.0f, this->windowRect.y + 102.0f,
                 100.0f, 30.0f},
                btnText.c_str())) {
    this->isOn = !this->isOn;
  }

  if (this->isOn != this->oldStateOn && isControl) {
    this->makeValue();
    this->isChangeValue = true;
  }

  this->oldStateOn = this->isOn;
  //-----

  // Show indicate
  if (this->leak.value >= this->limit) {
    this->indicateColor = RED;
  } else {
    this->indicateColor = GREEN;
  }
  //-----
}

void ClientDigitalTwin::Gas::parsingOption() {
  try {
    this->limit = this->options.at("limit");
    this->leak = {this->options.at("leak"), this->options.at("leak"), 0.0f,
                  100.0f};
  } catch (...) {
    this->limit = 5.0f;
    this->leak = {1.0f, 1.0f, 0.0f, 100.0f};
  }
}

void ClientDigitalTwin::Gas::makeValue() {
  this->sendValue["value"] = static_cast<float>(this->isOn);
}