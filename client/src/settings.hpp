#pragma once
#include <string>
#include <array>
#include <cstdint>

namespace ClientDigitalTwin {
extern const std::string CONFIG_FILE;
extern const int PING_INTERVAL;
enum Tag { HOME, SENSOR };
extern const std::array<std::string, 2> TAGS;
typedef uint16_t id_measure;

struct URL {
  std::string host;
  long port;
};

} // namespace ClientDigitalTwin