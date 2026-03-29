#include "manager.hpp"
#include <raygui/raygui.h>
#include <limits>
#include <iostream>

ClientDigitalTwin::Manager::Manager() {
  this->currentMode = ClientDigitalTwin::Mode::SIMULATION;
  this->dropdownMenu.active = false;
  this->dropdownMenu.options = "Simulation;Control";
  this->isCreateTableDB = false;
  this->isCreateSensorDB = false;
  this->isSensorDataAdded = false;
  this->requestID = 0;
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

void ClientDigitalTwin::Manager::EnableTableCreation() {
  this->isCreateTableDB = true;
}

bool ClientDigitalTwin::Manager::IsCreateTableDB() const {
  return this->isCreateTableDB;
}

void ClientDigitalTwin::Manager::MakeRoomIdentification(
    int id, int floor, const std::string &title) {
  this->rooms.push_back(Rooms{id, floor, title});
}

const int
ClientDigitalTwin::Manager::GetRoomID(const std::string &tileRoom) const {
  for (const auto room : this->rooms) {
    if (room.title == tileRoom) {
      return room.id;
    }
  }

  return -1;
}

void ClientDigitalTwin::Manager::AddSensorID(Sensor *sensor, int id) {
  this->sensorsID[sensor] = id;
}

bool ClientDigitalTwin::Manager::IsCreateSensorDB() const {
  return this->isCreateSensorDB;
}

void ClientDigitalTwin::Manager::EnableSensorCreation() {
  this->isCreateSensorDB = true;
}

const std::map<ClientDigitalTwin::Sensor *, int> &
ClientDigitalTwin::Manager::GetSensorsID() const {
  return this->sensorsID;
}

bool ClientDigitalTwin::Manager::IsSensorDataAdded() const {
  return this->isSensorDataAdded;
}

void ClientDigitalTwin::Manager::EnableSensorDataAdded() {
  this->isSensorDataAdded = true;
}

const size_t &ClientDigitalTwin::Manager::CountRequestID() {
  if (this->requestID == std::numeric_limits<size_t>::max()) {
    this->requestID = 0;
  }
  this->requestID++;

  return this->requestID;
}