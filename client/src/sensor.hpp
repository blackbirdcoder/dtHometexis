#pragma once
#include <string>
#include <raylib.h>
#include <raygui/raygui.h>

namespace ClientDigitalTwin {
class Sensor {
public:
  Sensor(std::string name, std::string type, std::string unit, double value,
         Vector3 position);
  std::string GetIndication() const; // Edit method
  void ShowWindow(const Camera3D &camera);
  void Draw(Model &model, float angle = 0.0f);
  Vector3 GetPosition();
  void SwitchOpenWindow();
  bool IsOpenWindow() const;
  const Rectangle &GetWindowRect() const;
  void ClickHandler(const Camera &camera, bool isBusyCursor);
  void DrawName(const Camera3D &camera) const;

private:
  bool openWindow;
  std::string name;
  std::string type;
  std::string unit;
  double value;
  Vector3 position;
  Rectangle windowRect;
  bool isDragging;
  Vector2 offset;
  float collisionRadius;
  Vector3 collisionPosition;
  Ray ray;
  RayCollision rayCollision;
};
} // namespace ClientDigitalTwin