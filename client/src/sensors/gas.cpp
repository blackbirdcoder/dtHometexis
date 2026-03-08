#include "gas.hpp"

ClientDigitalTwin::Gas::Gas(std::string name, std::string type,
                            std::string unit, float value, Vector3 position,
                            float angle, ClientDigitalTwin::Option options,
                            ClientDigitalTwin::Mode mode)
    : Sensor(name, type, unit, value, position, angle, options, mode) {
  this->isGas = static_cast<bool>(value);
}

void ClientDigitalTwin::Gas::ShowWindow(const Camera3D &camera) {
  Sensor::ShowWindow(camera);

  // Current value notifications
  std::string openText = this->type + ": " + (this->isGas ? "yes" : "no");
  GuiLabel(
      {this->windowRect.x + 5.0f, this->windowRect.y + 25.0f, 200.0f, 20.0f},
      openText.c_str());
  //-----

  // Let it smoke
  bool isControl = this->mode == ClientDigitalTwin::Mode::CONTROL;

  if (isControl) {
    GuiDisable();
  }
  std::string btnText = this->isGas ? "Stop gas" : "Start gas";
  if (GuiButton({this->windowRect.x + 143.0f, this->windowRect.y + 102.0f,
                 100.0f, 30.0f},
                btnText.c_str())) {
    this->isGas = !isGas;
  }
  if (isControl) {
    GuiEnable();
  }

  if (this->isGas) {
    this->indicateColor = RED;
  } else {
    this->indicateColor = GREEN;
  }
  //-----
}