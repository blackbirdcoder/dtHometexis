#include "door.hpp"

ClientDigitalTwin::Door::Door(std::string name, std::string type,
                              std::string unit, float value, Vector3 position,
                              float angle, ClientDigitalTwin::Option options,
                              ClientDigitalTwin::Mode mode)
    : Sensor(name, type, unit, value, position, angle, options, mode) {
  this->isOpen = static_cast<bool>(value);
  this->oldStateOpen = this->isOpen;
  this->parsingOption();
}

void ClientDigitalTwin::Door::ShowWindow(const Camera3D &camera) {
  Sensor::ShowWindow(camera);

  if (this->mode != this->oldMode) {
    this->isOpen = static_cast<bool>(value);
    this->parsingOption();
    this->oldMode = this->mode;
  }

  // Current value notifications
  std::string openText = this->type + ": " + (this->isOpen ? "open" : "close");
  GuiLabel(
      {this->windowRect.x + 5.0f, this->windowRect.y + 25.0f, 200.0f, 20.0f},
      openText.c_str());
  //-----

  // Selecting a option
  GuiCheckBox(
      {this->windowRect.x + 7.0f, this->windowRect.y + 55.0f, 15.0f, 15.0f},
      "Turn alarm", &this->isAlarm);

  if (this->isAlarm && this->isOpen) {
    this->indicateColor = RED;
  } else {
    this->indicateColor = GREEN;
  }

  bool isControl = this->mode == ClientDigitalTwin::Mode::CONTROL;

  if (this->isAlarm != this->oldStateAlarm && isControl) {
    this->makeOption();
    this->isChangeOption = true;
  }
  this->oldStateAlarm = this->isAlarm;
  //-----

  // Toggle door
  std::string btnText = this->isOpen ? "Close" : "Open";
  if (GuiButton({this->windowRect.x + 143.0f, this->windowRect.y + 102.0f,
                 100.0f, 30.0f},
                btnText.c_str())) {
    this->isOpen = !this->isOpen;
  }
  if (this->isOpen != this->oldStateOpen && isControl) {
    this->makeValue();
    this->isChangeValue = true;
  } else {
    //...
  }

  this->oldStateOpen = this->isOpen;
  //-----
}

void ClientDigitalTwin::Door::parsingOption() {
  try {
    this->isAlarm = static_cast<bool>(this->options.at("alarm"));
    this->oldStateAlarm = this - isAlarm;
  } catch (...) {
    this->isAlarm = false;
    this->oldStateAlarm = this->isAlarm;
  }
}

void ClientDigitalTwin::Door::makeOption() {
  this->sendOption["alarm"] = static_cast<float>(this->isAlarm);
}

void ClientDigitalTwin::Door::makeValue() {
  this->sendValue["value"] = static_cast<float>(this->isOpen);
}