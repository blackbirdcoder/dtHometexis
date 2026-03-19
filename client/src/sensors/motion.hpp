#pragma once
#include "sensor.hpp"

namespace ClientDigitalTwin {
class Motion : public Sensor {
public:
  Motion(std::string name, std::string type, std::string unit, float value,
         Vector3 position, float angle, Option options, Mode mode);
  void ShowWindow(const Camera3D &camera) override;

private:
  bool isMove;
  bool isTrack;
  bool oldStateTrack;
  bool isAlarm;
  bool oldStateAlarm;
  bool isSimulateMove;
  void parsingOption() override;
  void makeOption() override;
};
} // namespace ClientDigitalTwin