#include "window.hpp"

ClientDigitalTwin::Window::Window(std::string name, std::string type,
                                  std::string unit, double value,
                                  Vector3 position, float angle)
    : Sensor(name, type, unit, value, position, angle) {}

void ClientDigitalTwin::Window::ShowWindow(const Camera3D &camera) {
  Sensor::ShowWindow(camera);
  GuiLabel({this->windowRect.x, this->windowRect.y + 40.0f, 200.0f, 20.0f},
           this->test);
}