#pragma once
#include <raylib.h>

namespace ClientDigitalTwin {
class Camera {
public:
  Camera();
  const Camera3D &Get() const;
  void Handler();

private:
  struct Zoom {
    float base;
    float max;
    float min;
  } zoom;
  Camera3D camera;
  float sensitivity;
  float speed;
};
} // namespace ClientDigitalTwin