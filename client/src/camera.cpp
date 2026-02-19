#include "camera.hpp"
#include <raymath.h>
#include <iostream>

ClientDigitalTwin::Camera::Camera() {
  Camera3D camera = {0};
  camera.position = (Vector3){10.0f, 10.0f, 10.0f};
  camera.target = (Vector3){0.0f, 0.0f, 0.0f};
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;
  this->camera = camera;
  this->zoom = {0.0f, 22.0f, 12.0f};
  this->sensitivity = 1.0f;
  this->speed = 2.0f;
  this->currentDistance = 0.0f;
}

const Camera3D &ClientDigitalTwin::Camera::Get() const { return this->camera; }

void ClientDigitalTwin::Camera::Handler() {
  Vector3 move = {0.0f};
  Vector3 rot = {0.0f};
  float scalar = this->speed * GetFrameTime();

  if (IsKeyDown(KEY_W)) {
    Vector3 dir = {1.0f, 0.0f, 0.0f};
    move = Vector3Add(move, Vector3Scale(dir, scalar));

  } else if (IsKeyDown(KEY_S)) {
    Vector3 dir = {-1.0f, 0.0f, 0.0};
    move = Vector3Add(move, Vector3Scale(dir, scalar));

  } else if (IsKeyDown(KEY_D)) {
    Vector3 dir = {0.0f, 1.0f, 0.0};
    move = Vector3Add(move, Vector3Scale(dir, scalar));

  } else if (IsKeyDown(KEY_A)) {
    Vector3 dir = {0.0f, -1.0f, 0.0};
    move = Vector3Add(move, Vector3Scale(dir, scalar));
  }

  if (IsKeyPressed(KEY_ONE)) {
    this->currentDistance = 0.0f;
    this->camera.position = (Vector3){10.0f, 10.0f, 10.0f};
    this->camera.target = (Vector3){0.0f, 0.0f, 0.0f};
  }

  if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
    DisableCursor();
  }

  if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
    EnableCursor();
  }

  if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
    Vector2 mouseDelta = GetMouseDelta();
    rot.x = mouseDelta.x * this->sensitivity * GetFrameTime();
    rot.y = mouseDelta.y * this->sensitivity * GetFrameTime();
  }

  float wheel = GetMouseWheelMove();
  if (wheel != 0.0f) {
    Vector3 diff = Vector3Subtract(this->camera.position, this->camera.target);
    float dist = Vector3Length(diff);

    dist -= wheel;
    dist = Clamp(dist, this->zoom.min, this->zoom.max);
    diff = Vector3Normalize(diff);

    this->currentDistance = dist;
    this->camera.position =
        Vector3Add(this->camera.target, Vector3Scale(diff, dist));
  }

  UpdateCameraPro(&this->camera, move, rot, this->zoom.base);
}

const float ClientDigitalTwin::Camera::GetDistance() const {
  return this->currentDistance;
}