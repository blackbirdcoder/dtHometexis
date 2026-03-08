#include "smoke.hpp"

ClientDigitalTwin::Smoke::Smoke(std::string name, std::string type,
                                std::string unit, float value, Vector3 position,
                                float angle, ClientDigitalTwin::Option options,
                                ClientDigitalTwin::Mode mode)
    : Sensor(name, type, unit, value, position, angle, options, mode) {
  this->isSmoke = static_cast<bool>(value);
}

void ClientDigitalTwin::Smoke::ShowWindow(const Camera3D &camera) {
  Sensor::ShowWindow(camera);

  // Current value notifications
  std::string openText = this->type + ": " + (this->isSmoke ? "yes" : "no");
  GuiLabel(
      {this->windowRect.x + 5.0f, this->windowRect.y + 25.0f, 200.0f, 20.0f},
      openText.c_str());
  //-----

  // Let it smoke
  bool isControl = this->mode == ClientDigitalTwin::Mode::CONTROL;

  if (isControl) {
    GuiDisable();
  }
  std::string btnText = this->isSmoke ? "Stop smoke" : "Start smoke";
  if (GuiButton({this->windowRect.x + 143.0f, this->windowRect.y + 102.0f,
                 100.0f, 30.0f},
                btnText.c_str())) {
    this->isSmoke = !isSmoke;
  }
  if (isControl) {
    GuiEnable();
  }

  if (this->isSmoke) {
    this->indicateColor = RED;
  } else {
    this->indicateColor = GREEN;
  }
  //-----
}