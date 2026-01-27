#pragma once
#include <string>

namespace ServerDigitalTwin {
extern const std::string CONFIG_FILE;

struct URL {
  std::string host;
  long port;
  std::string dataPath;
};

} // namespace ServerDigitalTwin
