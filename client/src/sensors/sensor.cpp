#include "sensor.hpp"
#include <sstream>
#include <vector>

ClientDigitalTwin::Sensor::Sensor(std::string name, std::string type,
                                  std::string unit, float value,
                                  Vector3 position, float angle,
                                  ClientDigitalTwin::Option options,
                                  ClientDigitalTwin::Mode mode) {
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
  this->angle = angle;
  this->distanceLimit = 21.0f;
  this->windowSize = {250.0f, 140.0f};
  this->mode = mode;
  this->indicateColor = GREEN;
  this->danger = false;
  this->options = options;
  this->type[0] = std::toupper(this->type[0]);
  this->isChangeOption = false;
  this->isChangeValue = false;
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
          this->windowSize.width,
          this->windowSize.hight,
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
    drawIndication();
  }
}

void ClientDigitalTwin::Sensor::Draw(Model &model) {
  DrawModelEx(model, this->position,
              (Vector3){
                  0.0f,
                  1.0f,
                  0.0f,
              },
              this->angle,
              (Vector3){
                  1.0f,
                  1.0f,
                  1.0f,
              },
              WHITE);
  if (this->openWindow) {
    DrawCubeWires(
        {
            this->position.x,
            this->position.y + 0.5f,
            this->position.z,
        },
        0.5f, 1.0f, 0.5f, GREEN);
  }
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

void ClientDigitalTwin::Sensor::DrawName(const Camera3D &camera,
                                         const float distance) const {
  if (distance < this->distanceLimit) {
    Vector2 screenPos = GetWorldToScreen(
        {
            this->position.x,
            this->position.y,
            this->position.z,
        },
        camera);
    if (!this->openWindow) {
      screenPos.x -= 30.0f;
      screenPos.y -= 30.0f;
      GuiLabel(
          (Rectangle){
              screenPos.x,
              screenPos.y,
              120.0f,
              5.0f,
          },
          this->name.c_str());
    }
  }
}

void ClientDigitalTwin::Sensor::drawIndication() const {
  DrawCircle(this->windowRect.x + 235.0f, this->windowRect.y + 36.0f, 7.0f,
             GRAY);
  DrawCircle(this->windowRect.x + 235.0f, this->windowRect.y + 36.0f, 6.0f,
             this->indicateColor);
}

void ClientDigitalTwin::Sensor::ModeUpdate(Mode mode) {
  if (this->mode != mode) {
    this->mode = mode;
  }
}

void ClientDigitalTwin::Sensor::Update(float value, Option options) {
  this->value = value;
  this->options = options;
}

const std::string &ClientDigitalTwin::Sensor::GetName() const {
  return this->name;
}

void ClientDigitalTwin::Sensor::parsingOption() {}

void ClientDigitalTwin::Sensor::ResetChangeFlagOption() {
  this->isChangeOption = false;
}

const bool ClientDigitalTwin::Sensor::IsChangeOptions() const {
  return this->isChangeOption;
}

ClientDigitalTwin::Option ClientDigitalTwin::Sensor::GetNewOption() const {
  return this->sendOption;
}

void ClientDigitalTwin::Sensor::makeOption() {}

void ClientDigitalTwin::Sensor::ResetChangeFlagValue() {
  this->isChangeValue = false;
}

const bool ClientDigitalTwin::Sensor::IsChangeValue() const {
  return this->isChangeValue;
}

void ClientDigitalTwin::Sensor::makeValue() {}

ClientDigitalTwin::Option ClientDigitalTwin::Sensor::GetNewValue() const {
  return this->sendValue;
}

const float ClientDigitalTwin::Sensor::GetValue() const { return this->value; }

const std::string &ClientDigitalTwin::Sensor::GetType() const {
  return this->type;
}

std::string ClientDigitalTwin::Sensor::GetWhichRoom() {
  std::stringstream ss(this->name);
  std::string token;
  std::vector<std::string> tmp;
  while (std::getline(ss, token, '_')) {
    tmp.push_back(token);
  }
  tmp[0][0] = std::toupper(tmp[0][0]);

  return tmp[0];
}