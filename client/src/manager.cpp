#include "manager.hpp"

ClientDigitalTwin::Manager::Manager() {
  this->currentMode = ClientDigitalTwin::Mode::SIMULATION;
}

void ClientDigitalTwin::Manager::SelectOperatingMode() {}

const ClientDigitalTwin::Mode ClientDigitalTwin::Manager::GetMode() const {
  return this->currentMode;
}