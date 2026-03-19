#pragma once
#include "settings.hpp"
#include <string>

namespace ClientDigitalTwin {
class Manager {
public:
  Manager();
  void SelectOperatingMode();
  const Mode GetMode() const;
  void DrawTextMode() const;

private:
  Mode currentMode;
  struct {
    int id;
    bool active;
    std::string options;
  } dropdownMenu;
};
} // namespace ClientDigitalTwin