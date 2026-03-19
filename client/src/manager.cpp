#include "manager.hpp"
#include <raygui/raygui.h>
#include <iostream>

ClientDigitalTwin::Manager::Manager() {
  this->currentMode = ClientDigitalTwin::Mode::SIMULATION;
  this->dropdownMenu.active = false;
  this->dropdownMenu.options = "Simulation;Control";
}

void ClientDigitalTwin::Manager::SelectOperatingMode() {
  if (this->dropdownMenu.active) {
    GuiLock();
  }
  if (GuiDropdownBox((Rectangle){20.0f, 10.0f, 150.0f, 30.0f},
                     dropdownMenu.options.c_str(), &this->dropdownMenu.id,
                     this->dropdownMenu.active)) {
    this->currentMode =
        static_cast<ClientDigitalTwin::Mode>(this->dropdownMenu.id);
    this->dropdownMenu.active = !this->dropdownMenu.active;
  }
  GuiUnlock();
}

const ClientDigitalTwin::Mode ClientDigitalTwin::Manager::GetMode() const {
  return this->currentMode;
}

void ClientDigitalTwin::Manager::DrawTextMode() const {
  Font font = GuiGetFont();
  std::string modeText = "Mode: ";
  modeText += this->currentMode == ClientDigitalTwin::Mode::SIMULATION
                  ? "Simulation"
                  : "Control";
  GuiSetStyle(DEFAULT, TEXT_SIZE, font.baseSize + 1);
  GuiLabel((Rectangle){static_cast<float>(GetScreenWidth() / 2.0f),
                       GetScreenHeight() - 40.0f, 150.0f, 30.0f},
           modeText.c_str());
  GuiSetStyle(DEFAULT, TEXT_SIZE, font.baseSize);
}