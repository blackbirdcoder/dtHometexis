#include "storage.hpp"

ClientDigitalTwin::Storage::Storage(const std::string &dbName)
    : database(dbName, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {}

void ClientDigitalTwin::Storage::Init() {
  database.exec("PRAGMA foreign_keys = ON;");

  database.exec(R"(
    CREATE TABLE IF NOT EXISTS House (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT
    );
)");

  database.exec(R"(
    CREATE TABLE IF NOT EXISTS Room (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        house_id INTEGER,
        name TEXT,
        floor INTEGER,
        FOREIGN KEY(house_id) REFERENCES House(id)
    );
)");

  database.exec(R"(
    CREATE TABLE IF NOT EXISTS Sensor (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        room_id INTEGER,
        type TEXT,
        FOREIGN KEY(room_id) REFERENCES Room(id)
    );
)");

  database.exec(R"(
    CREATE TABLE IF NOT EXISTS SensorData (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        sensor_id INTEGER,
        value FLOAT,
        timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
        FOREIGN KEY(sensor_id) REFERENCES Sensor(id)
    );
)");
}

SQLite::Database *ClientDigitalTwin::Storage::Get() { return &database; }

int ClientDigitalTwin::Storage::CreateHome(const std::string &name) {
  SQLite::Statement query(this->database,
                          "SELECT id FROM House WHERE name = ? LIMIT 1;");
  query.bind(1, name);

  if (query.executeStep()) {
    auto id = query.getColumn(0).getInt();
    query.reset();
    return id;
  }
  query.reset();
  return addHome(name);
}

int ClientDigitalTwin::Storage::CreateRoom(int homeID, const std::string &name,
                                           int floor) {
  SQLite::Statement query(
      this->database,
      "SELECT id FROM Room WHERE house_id = ? AND name = ? LIMIT 1;");
  query.bind(1, homeID);
  query.bind(2, name);

  if (query.executeStep()) {
    auto id = query.getColumn(0).getInt();
    query.reset();
    return id;
  }
  query.reset();
  return addRoom(homeID, name, floor);
}

int ClientDigitalTwin::Storage::CreateSensor(int roomID,
                                             const std::string &name) {
  SQLite::Statement query(
      this->database,
      "SELECT id FROM Sensor WHERE room_id = ? AND type = ? LIMIT 1;");
  query.bind(1, roomID);
  query.bind(2, name);

  if (query.executeStep()) {
    auto id = query.getColumn(0).getInt();
    query.reset();
    return id;
  }

  query.reset();
  return addSensor(roomID, name);
}

int ClientDigitalTwin::Storage::addHome(const std::string &name) {
  SQLite::Statement query(this->database,
                          "INSERT INTO House (name) VALUES (?);");
  query.bind(1, name);
  query.exec();
  query.reset();
  return this->database.getLastInsertRowid();
}

int ClientDigitalTwin::Storage::addRoom(int homeID, const std::string &name,
                                        int floor) {
  SQLite::Statement query(
      this->database,
      "INSERT INTO Room (house_id, name, floor) VALUES (?, ?, ?);");
  query.bind(1, homeID);
  query.bind(2, name);
  query.bind(3, floor);
  query.exec();
  query.reset();
  return this->database.getLastInsertRowid();
}

int ClientDigitalTwin::Storage::addSensor(int roomID, const std::string &name) {
  SQLite::Statement query(this->database,
                          "INSERT INTO Sensor (room_id, type) VALUES (?, ?);");
  query.bind(1, roomID);
  query.bind(2, name);
  query.exec();
  query.reset();
  return this->database.getLastInsertRowid();
}

void ClientDigitalTwin::Storage::AddSensorData(int sensorID, float value) {
  SQLite::Statement query(
      this->database,
      "INSERT INTO SensorData (sensor_id, value) VALUES (?, ?);");
  query.bind(1, sensorID);
  query.bind(2, value);
  query.exec();
  query.reset();
}
