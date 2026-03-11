#include "manager.hpp"
#include <raygui/raygui.h>
#include <iostream>

ClientDigitalTwin::Manager::Manager() {
  this->currentMode = ClientDigitalTwin::Mode::SIMULATION;
  this->dropdownMenu.active = false;
}

void ClientDigitalTwin::Manager::SelectOperatingMode() {
  if (this->dropdownMenu.active) {
    GuiLock();
  }
  if (GuiDropdownBox((Rectangle){10.0f, 10.0f, 150.0f, 30.0f},
                     "Simulation;Control", &this->dropdownMenu.id,
                     this->dropdownMenu.active)) {
    switch (this->dropdownMenu.id) {
    case 0:
      this->currentMode = ClientDigitalTwin::Mode::SIMULATION;
      break;
    case 1:
      this->currentMode = ClientDigitalTwin::Mode::CONTROL;
      break;
    }
    this->dropdownMenu.active = !this->dropdownMenu.active;
  }
  std::cout << "(*) CURRENT MODE: " << static_cast<int>(this->currentMode)
            << '\n';
  GuiUnlock();
}

const ClientDigitalTwin::Mode ClientDigitalTwin::Manager::GetMode() const {
  return this->currentMode;
}