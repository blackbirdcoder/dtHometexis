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
  struct {
    int id;
    bool active;
  } dropdownMenu;
};
} // namespace ClientDigitalTwin