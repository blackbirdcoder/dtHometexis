#pragma once
#include <string>

namespace ClientDigitalTwin {
extern const std::string CONFIG_FILE;

struct URL {
  std::string host;
  long port;
};
} // namespace ClientDigitalTwin