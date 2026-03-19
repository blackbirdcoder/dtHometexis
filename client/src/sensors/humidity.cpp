#include "humidity.hpp"
#include <cmath>
#include <cstring>

ClientDigitalTwin::Humidity::Humidity(std::string name, std::string type,
                                      std::string unit, float value,
                                      Vector3 position, float angle,
                                      ClientDigitalTwin::Option options,
                                      ClientDigitalTwin::Mode mode)
    : Sensor(name, type, unit, value, position, angle, options, mode) {
  this->indicationLeak = 90;
  this->humidity = 0.0f;
  this->oldHumidity = this->humidity;
  this->humidityForSimulation = std::round(this->value);
  this->parsingOption();
}

void ClientDigitalTwin::Humidity::ShowWindow(const Camera3D &camera) {
  Sensor::ShowWindow(camera);

  if (this->mode != this->oldMode) {
    this->parsingOption();
    this->oldMode = this->mode;
  }

  this->humidity = static_cast<uint8_t>(std::round(this->value));
  if (this->oldHumidity != this->humidity) {
    this->humidityForSimulation = static_cast<float>(this->humidity);
    this->oldHumidity = this->humidity;
  }

  bool isSimulation = this->mode == ClientDigitalTwin::Mode::SIMULATION;

  if (isSimulation) {
    this->humidity = this->humidityForSimulation;
  }

  // Current value notifications
  std::string humidityText =
      this->type + ": " + std::to_string(this->humidity) + "%";
  GuiLabel(
      {this->windowRect.x + 5.0f, this->windowRect.y + 25.0f, 200.0f, 20.0f},
      humidityText.c_str());
  //-----

  bool isControl = this->mode == ClientDigitalTwin::Mode::CONTROL;

  // Selecting a comfort mode
  GuiCheckBox(
      {this->windowRect.x + 7.0f, this->windowRect.y + 55.0f, 15.0f, 15.0f},
      "Air humidifier", &this->humidifier.humify);

  if (this->humidifier.humify != this->humidifier.oldState && isControl) {
    this->makeOption();
    this->isChangeOption = true;
  } else {
    //...
  }
  this->humidifier.oldState = this->humidifier.humify;

  if (!this->humidifier.humify) {
    GuiDisable();
  }
  if (GuiButton({this->windowRect.x + 110.0f, this->windowRect.y + 55.0f, 20.0f,
                 20.0f},
                "-")) {
    uint8_t val = std::stoi(this->humidifier.percentText);
    if (val > this->humidifier.min) {
      --val;
      strcpy(this->humidifier.percentText, std::to_string(val).c_str());

      if (isControl) {
        this->makeOption();
        this->isChangeOption = true;
      }
    }
  }
  GuiTextBox(
      {this->windowRect.x + 135.0f, this->windowRect.y + 55.0f, 35.0f, 20.0f},
      this->humidifier.percentText, 14, false);
  if (GuiButton({this->windowRect.x + 175.0f, this->windowRect.y + 55.0f, 20.0f,
                 20.0f},
                "+")) {
    uint8_t val = std::stoi(this->humidifier.percentText);
    if (val < this->humidifier.max) {
      ++val;
      strcpy(this->humidifier.percentText, std::to_string(val).c_str());

      if (isControl) {
        this->makeOption();
        this->isChangeOption = true;
      }
    }
  }
  GuiLabel(
      {this->windowRect.x + 197.0f, this->windowRect.y + 55.0f, 20.0f, 20.0f},
      "%");
  if (!this->humidifier.humify) {
    GuiEnable();
  }
  //-----

  // Displaying a warning
  std::string waterLeakText = "Water leak: no";
  this->danger = false;
  this->indicateColor = GREEN;
  if (humidity > this->indicationLeak) {
    this->danger = true;
    waterLeakText = "Water leak: yes";
    this->indicateColor = RED;
  }
  GuiLabel(
      {this->windowRect.x + 5.0f, this->windowRect.y + 80.0f, 200.0f, 20.0f},
      waterLeakText.c_str());
  //-----

  // Set values ​​for simulation

  if (isControl) {
    GuiDisable();
    this->humidityForSimulation = humidity;
  }
  GuiSlider(
      {this->windowRect.x + 7.0f, this->windowRect.y + 110.0f, 100.0f, 15.0f},
      NULL, TextFormat("%i%%", static_cast<int>(this->humidityForSimulation)),
      &this->humidityForSimulation, 0, 100);
  this->humidity = this->humidityForSimulation;
  if (isControl) {
    GuiEnable();
  }
  //-----
}

void ClientDigitalTwin::Humidity::parsingOption() {
  try {
    this->humidifier.humify = static_cast<bool>((this->options.at("humify")));
    this->humidifier.oldState = this->humidifier.humify;
    this->humidifier.max = this->options.at("max");
    this->humidifier.min = this->options.at("min");
    std::string val =
        std::to_string(static_cast<int>((this->options.at("recommended"))));
    strcpy(this->humidifier.percentText, val.c_str());
  } catch (...) {
    this->humidifier.humify = true;
    this->humidifier.oldState = this->humidifier.humify;
    this->humidifier.min = 40;
    this->humidifier.max = 60;
    strcpy(this->humidifier.percentText, "50");
  }
}

void ClientDigitalTwin::Humidity::makeOption() {
  this->sendOption["humify"] = static_cast<float>(this->humidifier.humify);
  this->sendOption["recommended"] =
      static_cast<float>(std::stoi(this->humidifier.percentText));
}