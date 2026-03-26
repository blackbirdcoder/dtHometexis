#pragma once
#include <string>
#include <SQLiteCpp/Database.h>
#include "settings.hpp"

namespace ClientDigitalTwin {
class Storage final {
public:
  static Storage &Instance() {
    static Storage instance(DB_NAME);
    return instance;
  }
  void Init();
  Storage(const Storage &) = delete;
  Storage &operator=(const Storage &) = delete;
  SQLite::Database *Get();
  int CreateHome(const std::string &name);
  int CreateRoom(int homeID, const std::string &name, int floor);
  int CreateSensor(int roomID, const std::string &name);
  void AddSensorData(int sensorID, float value);
  
  private:
  int addHome(const std::string &name);
  int addRoom(int homeID, const std::string &name, int floor);
  int addSensor(int roomID, const std::string &name);
  Storage(const std::string &dbName);
  SQLite::Database database;
};
} // namespace ClientDigitalTwin