#include "temperature.hpp"
#include <cmath>
#include <cstring>

ClientDigitalTwin::Temperature::Temperature(std::string name, std::string type,
                                            std::string unit, float value,
                                            Vector3 position, float angle,
                                            ClientDigitalTwin::Option options,
                                            ClientDigitalTwin::Mode mode)
    : Sensor(name, type, unit, value, position, angle, options, mode) {
  this->indicationFire = 90;
  this->celsius = static_cast<int>(std::round(this->value));
  this->tempForSimulation = static_cast<float>(this->celsius);

  try {
    this->roomHeat.isHeating = static_cast<bool>(this->options.at("heating"));
    this->roomHeat.min = this->options.at("min");
    this->roomHeat.max = this->options.at("max");
    std::string val =
        std::to_string(static_cast<int>(this->options.at("recommended")));
    strcpy(this->roomHeat.celsiusText, val.c_str());
  } catch (...) {
    this->roomHeat.isHeating = false;
    this->roomHeat.min = 5;
    this->roomHeat.max = 30;
    strcpy(this->roomHeat.celsiusText, "10");
  }
}

void ClientDigitalTwin::Temperature::ShowWindow(const Camera3D &camera) {
  Sensor::ShowWindow(camera);

  // Current value notifications
  std::string tempText =
      this->type + ": " + std::to_string(this->celsius) + "'C";
  GuiLabel(
      {this->windowRect.x + 5.0f, this->windowRect.y + 25.0f, 200.0f, 20.0f},
      tempText.c_str());
  //-----

  // Selecting a comfort mode
  GuiCheckBox(
      {this->windowRect.x + 7.0f, this->windowRect.y + 55.0f, 15.0f, 15.0f},
      "Room heating", &this->roomHeat.isHeating);

  if (this->roomHeat.isHeating) {
    //...
  } else {
    //...
  }
  if (!this->roomHeat.isHeating) {
    GuiDisable();
  }
  if (GuiButton({this->windowRect.x + 110.0f, this->windowRect.y + 55.0f, 20.0f,
                 20.0f},
                "-")) {
    uint8_t val = std::stoi(this->roomHeat.celsiusText);
    if (val > this->roomHeat.min) {
      --val;
      strcpy(this->roomHeat.celsiusText, std::to_string(val).c_str());
    }
  }
  GuiTextBox(
      {this->windowRect.x + 135.0f, this->windowRect.y + 55.0f, 35.0f, 20.0f},
      this->roomHeat.celsiusText, 14, false);
  if (GuiButton({this->windowRect.x + 175.0f, this->windowRect.y + 55.0f, 20.0f,
                 20.0f},
                "+")) {
    uint8_t val = std::stoi(this->roomHeat.celsiusText);
    if (val < this->roomHeat.max) {
      ++val;
      strcpy(this->roomHeat.celsiusText, std::to_string(val).c_str());
    }
  }
  GuiLabel(
      {this->windowRect.x + 197.0f, this->windowRect.y + 55.0f, 20.0f, 20.0f},
      "'C");
  if (!this->roomHeat.isHeating) {
    GuiEnable();
  }
  //-----

  // Displaying a warning
  std::string fireText = "Fire: no";
  this->danger = false;
  this->indicateColor = GREEN;
  if (this->celsius > this->indicationFire) {
    this->danger = true;
    fireText = "Fire: yes";
    this->indicateColor = RED;
  }
  GuiLabel(
      {this->windowRect.x + 5.0f, this->windowRect.y + 80.0f, 200.0f, 20.0f},
      fireText.c_str());
  //-----

  // Set values ​​for simulation
  bool isControl = this->mode == ClientDigitalTwin::Mode::CONTROL;

  if (isControl) {
    GuiDisable();
  }
  GuiSlider(
      {this->windowRect.x + 7.0f, this->windowRect.y + 110.0f, 100.0f, 15.0f},
      NULL, TextFormat("%i'C", static_cast<int>(this->tempForSimulation)),
      &this->tempForSimulation, -55, 125);
  this->celsius = this->tempForSimulation;
  if (isControl) {
    GuiEnable();
  }
  //-----
}