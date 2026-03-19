#pragma once
#include <string>
#include <cstdint>
#include <map>

namespace ServerDigitalTwin {
extern const std::string CONFIG_FILE;
typedef std::map<std::string, float> Option;

struct URL {
  std::string host;
  uint16_t port;
};

} // namespace ServerDigitalTwin
