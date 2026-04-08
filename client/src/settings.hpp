#pragma once
#include <string>
#include <array>
#include <map>

namespace ClientDigitalTwin {
extern const std::string CONFIG_FILE;
extern const int PING_INTERVAL;
extern const int SIZE;
enum Tag { HOME_WALL_SOLID, SENSOR, UPDATE, SET, ROOM, HOME_OPENING };
extern const std::array<std::string, 6> TAGS;
enum class Mode { SIMULATION, CONTROL };
typedef std::map<std::string, float> Option;

struct URL {
  std::string host;
  long port;
};

extern const std::string DB_NAME;
} // namespace ClientDigitalTwin