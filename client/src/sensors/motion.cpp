#include "motion.hpp"

ClientDigitalTwin::Motion::Motion(std::string name, std::string type,
                                  std::string unit, float value,
                                  Vector3 position, float angle,
                                  ClientDigitalTwin::Option options,
                                  ClientDigitalTwin::Mode mode)
    : Sensor(name, type, unit, value, position, angle, options, mode) {
  this->isMove = value;
  this->isSimulateMove = false;
  this->parsingOption();
}

void ClientDigitalTwin::Motion::ShowWindow(const Camera3D &camera) {
  Sensor::ShowWindow(camera);

  if (this->mode != this->oldMode) {
    this->parsingOption();
    this->oldMode = this->mode;
  }

  // Current value notifications
  if (!this->isTrack) {
    GuiDisable();
    this->indicateColor = GRAY;
  } else {
    this->indicateColor = GREEN;
  }
  std::string moveText = this->type + ": " + (this->isMove ? "yes" : "no");
  GuiLabel(
      {this->windowRect.x + 5.0f, this->windowRect.y + 55.0f, 200.0f, 20.0f},
      moveText.c_str());
  if (!this->isTrack) {
    GuiEnable();
  }
  //-----

  bool isControl = this->mode == ClientDigitalTwin::Mode::CONTROL;

  // Selecting a option
  GuiCheckBox(
      {this->windowRect.x + 7.0f, this->windowRect.y + 30.0f, 15.0f, 15.0f},
      "Track motion", &this->isTrack);

  if (this->isTrack != this->oldStateTrack && isControl) {
    this->makeOption();
    this->isChangeOption = true;
  } else {
    //...
  }
  this->oldStateTrack = this->isTrack;

  if (!this->isTrack) {
    GuiDisable();
  }
  GuiCheckBox(
      {this->windowRect.x + 7.0f, this->windowRect.y + 85.0f, 15.0f, 15.0f},
      "Turn alarm", &this->isAlarm);
  if (this->isAlarm && this->isMove) {
    this->indicateColor = RED;
  } else {
    //...
  }

  if (this->isAlarm != this->oldStateAlarm && isControl) {
    this->makeOption();
    this->isChangeOption = true;
  }

  this->oldStateAlarm = this->isAlarm;

  if (!this->isTrack) {
    GuiEnable();
  }
  //-----

  // Set work simulation

  if (isControl) {
    GuiDisable();
  }
  if (GuiButton({this->windowRect.x + 143.0f, this->windowRect.y + 102.0f,
                 100.0f, 30.0f},
                "Test walk")) {
    this->isSimulateMove = true;
  }
  if (this->isSimulateMove) {
    this->isSimulateMove = false;
  }
  if (isControl) {
    GuiEnable();
  }
  //-----
}

void ClientDigitalTwin::Motion::parsingOption() {
  try {
    this->isTrack = static_cast<bool>(this->options.at("track"));
    this->isAlarm = static_cast<bool>(this->options.at("alarm"));
  } catch (...) {
    this->isTrack = true;
    this->isAlarm = false;
  }
}

void ClientDigitalTwin::Motion::makeOption() {
  this->sendOption["track"] = static_cast<float>(this->isTrack);
  this->sendOption["alarm"] = static_cast<float>(this->isAlarm);
}
