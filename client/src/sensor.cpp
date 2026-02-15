#include "sensor.hpp"
#include <sstream>
#include <iostream>

ClientDigitalTwin::Sensor::Sensor(std::string name, std::string type,
                                  std::string unit, double value,
                                  Vector3 position) {
  this->name = name;
  this->type = type;
  this->unit = unit;
  this->value = value;
  this->position = position;
  this->isDragging = false;
  this->windowRect = {0.0f, 0.0f, 0.0f, 0.0f};
  this->openWindow = false;
  this->collisionPosition = {position.x, position.y + 0.5f, position.z};
  this->collisionRadius = 0.4f;
  this->ray = {0.0f};
  this->rayCollision = {0};
}

void ClientDigitalTwin::Sensor::ClickHandler(const Camera &camera,
                                             bool isBusyCursor) {
  if (isBusyCursor) {
    return;
  }

  if (!this->openWindow) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      this->ray = GetScreenToWorldRay(GetMousePosition(), camera);
      if (!this->rayCollision.hit) {
        this->rayCollision = GetRayCollisionSphere(
            this->ray, this->collisionPosition, this->collisionRadius);
      }
      if (this->rayCollision.hit) {
        this->openWindow = true;
        this->rayCollision.hit = false;
      }
    }
  }
}

void ClientDigitalTwin::Sensor::ShowWindow(const Camera3D &camera) {
  if (this->openWindow) {
    Vector2 screenPos = GetWorldToScreen(this->position, camera);
    if (this->windowRect.width == 0) {
      this->windowRect = {
          screenPos.x + 20.0f,
          screenPos.y,
          250.0f,
          140.0f,
      };
    }

    Vector2 mouse = GetMousePosition();
    Rectangle title = {
        this->windowRect.x,
        this->windowRect.y,
        this->windowRect.width,
        24.0f,
    };

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        CheckCollisionPointRec(mouse, title)) {
      this->isDragging = true;
      offset = {mouse.x - this->windowRect.x, mouse.y - this->windowRect.y};
    }

    if (this->isDragging) {
      if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        this->windowRect.x = mouse.x - offset.x;
        this->windowRect.y = mouse.y - offset.y;
      } else {
        this->isDragging = false;
      }
    }

    if (GuiWindowBox(this->windowRect, this->name.c_str())) {
      windowRect = {-50.0f, -50.0f, 0.0f, 0.0f};
      this->openWindow = false;
    }
    GuiLabel((Rectangle){this->windowRect.x, this->windowRect.y + 20.0f, 200.0f,
                         20.0f},
             this->GetIndication().c_str());
  }
}

std::string ClientDigitalTwin::Sensor::GetIndication() const {
  std::ostringstream str;

  if (this->unit != "bool") {
    str << this->type << ":" << this->value << this->unit;
  } else {
    str << this->type << ":" << (this->value ? "on" : "off");
  }

  return str.str();
}

void ClientDigitalTwin::Sensor::Draw(Model &model, float angle) {
  DrawModelEx(model, this->position,
              (Vector3){
                  0.0f,
                  1.0f,
                  0.0f,
              },
              angle,
              (Vector3){
                  1.0f,
                  1.0f,
                  1.0f,
              },
              WHITE);
}

Vector3 ClientDigitalTwin::Sensor::GetPosition() { return this->position; }

void ClientDigitalTwin::Sensor::SwitchOpenWindow() {
  this->openWindow = !this->openWindow;
}

bool ClientDigitalTwin::Sensor::IsOpenWindow() const {
  return this->openWindow;
}

const Rectangle &ClientDigitalTwin::Sensor::GetWindowRect() const {
  return this->windowRect;
}