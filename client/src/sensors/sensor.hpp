#pragma once
#include <string>
#include <raylib.h>
#include <raygui/raygui.h>
#include "settings.hpp"
#include <map>

namespace ClientDigitalTwin {
class Sensor {
public:
  Sensor(std::string name, std::string type, std::string unit, float value,
         Vector3 position, float angle, Option options, Mode mode);
  // std::string GetIndication() const; // Edit method
  void virtual ShowWindow(const Camera3D &camera);
  void Draw(Model &model);
  Vector3 GetPosition();
  void SwitchOpenWindow();
  bool IsOpenWindow() const;
  const Rectangle &GetWindowRect() const;
  void ClickHandler(const Camera &camera, bool isBusyCursor);
  void DrawName(const Camera3D &camera, const float distance) const;

protected:
  Mode mode;
  bool openWindow;
  std::string name;
  std::string type;
  std::string unit;
  float value;
  Vector3 position;
  Rectangle windowRect;
  bool isDragging;
  Vector2 offset;
  float collisionRadius;
  Vector3 collisionPosition;
  Ray ray;
  RayCollision rayCollision;
  float angle;
  float distanceLimit;
  struct WindowSize {
    float width;
    float hight;
  } windowSize;
  Color indicateColor;
  bool danger;
  std::map<std::string, float> options;

  void drawIndication() const;
};
} // namespace ClientDigitalTwin