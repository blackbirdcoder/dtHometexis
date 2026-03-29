#pragma once
#include "settings.hpp"
#include <string>
#include <vector>
#include <map>
#include "sensor.hpp"

namespace ClientDigitalTwin {
class Manager {
public:
  Manager();
  void SelectOperatingMode();
  const Mode GetMode() const;
  void DrawTextMode() const;
  void EnableTableCreation();
  bool IsCreateTableDB() const;
  void MakeRoomIdentification(int id, int floor, const std::string &title);
  const int GetRoomID(const std::string &titleRoom) const;
  void AddSensorID(Sensor *sensor, int id);
  bool IsCreateSensorDB() const;
  void EnableSensorCreation();
  const std::map<Sensor *, int> &GetSensorsID() const;
  bool IsSensorDataAdded() const;
  void EnableSensorDataAdded();
  const size_t &GetRequestID();

private:
  Mode currentMode;
  bool isCreateTableDB;
  bool isCreateSensorDB;
  bool isSensorDataAdded;
  struct {
    int id;
    bool active;
    std::string options;
  } dropdownMenu;
  struct Rooms {
    int id;
    int floor;
    std::string title;
  };
  std::vector<Rooms> rooms;
  std::map<Sensor *, int> sensorsID;
  size_t requestID;
};
} // namespace ClientDigitalTwin