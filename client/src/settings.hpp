#pragma once
#include <string>
#include <array>
#include <cstdint>
#include <map>

namespace ClientDigitalTwin {
extern const std::string CONFIG_FILE;
extern const int PING_INTERVAL;
extern const int SIZE;
enum Tag { HOME, SENSOR, UPDATE, SET, ROOM };
extern const std::array<std::string, 5> TAGS;
typedef size_t id_measure;
enum class Mode { SIMULATION, CONTROL };
typedef std::map<std::string, float> Option;

struct URL {
  std::string host;
  long port;
};

extern const std::string DB_NAME;
} // namespace ClientDigitalTwin