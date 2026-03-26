#pragma once
#include <string>
#include <raylib.h>
#include <raygui/raygui.h>
#include "settings.hpp"

namespace ClientDigitalTwin {
class Sensor {
public:
  Sensor(std::string name, std::string type, std::string unit, float value,
         Vector3 position, float angle, Option options, Mode mode);
  void virtual ShowWindow(const Camera3D &camera);
  void Draw(Model &model);
  Vector3 GetPosition();
  void SwitchOpenWindow();
  bool IsOpenWindow() const;
  const Rectangle &GetWindowRect() const;
  void ClickHandler(const Camera &camera, bool isBusyCursor);
  void DrawName(const Camera3D &camera, const float distance) const;
  void ModeUpdate(Mode mode);
  void Update(float value, Option options);
  const std::string &GetName() const;
  virtual ~Sensor() = default;
  void ResetChangeFlagOption();
  const bool IsChangeOptions() const;
  Option GetNewOption() const;
  void ResetChangeFlagValue();
  const bool IsChangeValue() const;
  Option GetNewValue() const;
  const float GetValue() const;
  const std::string &GetType() const;
  std::string GetWhichRoom();

protected:
  Mode mode;
  Mode oldMode;
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
  Option options;
  bool isChangeOption;
  Option sendOption;
  Option sendValue;
  bool isChangeValue;
  void drawIndication() const;
  void virtual parsingOption();
  void virtual makeOption();
  void virtual makeValue();
};
} // namespace ClientDigitalTwin