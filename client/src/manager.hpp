#pragma once 
#include "settings.hpp"

namespace ClientDigitalTwin {
class Manager {
public:
  Manager();
  void SelectOperatingMode();
  const Mode GetMode() const;
private:
  Mode currentMode;
};
} // namespace ClientDigitalTwin