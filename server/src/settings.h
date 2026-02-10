#pragma once
#include <string>
#include <cstdint>

namespace ServerDigitalTwin {
extern const std::string CONFIG_FILE;

struct URL {
  std::string host;
  uint16_t port;
};

} // namespace ServerDigitalTwin
