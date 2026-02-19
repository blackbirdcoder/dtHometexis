#pragma once
#include <raylib.h>

namespace ClientDigitalTwin {
class Camera {
public:
  Camera();
  const Camera3D &Get() const;
  void Handler();
  const float GetDistance() const;

private:
  struct Zoom {
    float base;
    float max;
    float min;
  } zoom;
  Camera3D camera;
  float sensitivity;
  float speed;
  float currentDistance;
};
} // namespace ClientDigitalTwin