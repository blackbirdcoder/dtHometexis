#pragma once
#include <string>
#include <array>
#include <cstdint>
#include <map>

namespace ClientDigitalTwin {
extern const std::string CONFIG_FILE;
extern const int PING_INTERVAL;
extern const int SIZE;
enum Tag { HOME, SENSOR };
extern const std::array<std::string, 2> TAGS;
typedef uint16_t id_measure;
enum class Mode { SIMULATION, CONTROL };
typedef std::map<std::string, float> Option;

struct URL {
  std::string host;
  long port;
};

} // namespace ClientDigitalTwin