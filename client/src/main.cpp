#include <iostream>
#include <raylib.h>
#include <string>
#include <inih/INIReader.h>
#include <ixwebsocket/IXWebSocket.h>
#include "settings.hpp"
#include "client.hpp"
#include <vector>
#define RAYGUI_IMPLEMENTATION
#include <raygui/raygui.h>

// #include <nlohmann/json.hpp>
// using json = nlohmann::json;

int main() {
  std::vector<ClientDigitalTwin::Sensor> sensors;
  INIReader reader(ClientDigitalTwin::CONFIG_FILE);

  //--- Camera
  Camera3D camera = {0};
  camera.position = (Vector3){10.0f, 10.0f, 10.0f};
  camera.target = (Vector3){0.0f, 0.0f, 0.0f};
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;
  //---

  //---
  Vector3 cubePosition = {1.0f, 0.0f, 0.0f};
  Vector3 cubeSize = {0.5f, 0.5f, 0.5f};
  bool isOpenPanel = false;
  //---

  //---
  Ray ray = {0};
  RayCollision collision = {0};
  //---

  if (reader.ParseError() < 0) {
    std::cout << "[!] Error: Can't load"
              << "'" << ClientDigitalTwin::CONFIG_FILE << "'" << '\n';
    return 1;
  }

  ClientDigitalTwin::URL url = {
      reader.Get("server", "host", "0.0.0.0"),
      reader.GetInteger("server", "port", 5000),
  };

  ClientDigitalTwin::Client client(url, ClientDigitalTwin::PING_INTERVAL);
  client.Handler(sensors);
  client.Run();

  InitWindow(1024, 768, "Client Digital House");
  SetTargetFPS(60);
  GuiLoadStyle("assets/style/style_terminal.rgs");

  std::string t = ClientDigitalTwin::TAGS[ClientDigitalTwin::Tag::SENSOR];
  std::string m = "GetDataSensors";
  std::vector<std::string> v = {"parA", "parB"};
  ClientDigitalTwin::id_measure id = 2;
  client.Send(m, v, t, id);

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_H)) {
      std::string t = ClientDigitalTwin::TAGS[ClientDigitalTwin::Tag::HOME];
      std::string m = "test";
      std::vector<std::string> v = {"par1", "par2"};
      ClientDigitalTwin::id_measure id = 1;
      client.Send(m, v, t, id);
    }
    if (IsKeyPressed(KEY_B)) {
      std::string t = ClientDigitalTwin::TAGS[ClientDigitalTwin::Tag::SENSOR];
      std::string m = "GetDataSensors";
      std::vector<std::string> v = {"parA", "parB"};
      ClientDigitalTwin::id_measure id = 2;
      client.Send(m, v, t, id);
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      ray = GetScreenToWorldRay(GetMousePosition(), camera);
      if (!collision.hit) {
        collision = GetRayCollisionBox(
            ray, (BoundingBox){(Vector3){cubePosition.x - cubeSize.x / 2,
                                         cubePosition.y - cubeSize.y / 2,
                                         cubePosition.z - cubeSize.z / 2},
                               (Vector3){cubePosition.x + cubeSize.x / 2,
                                         cubePosition.y + cubeSize.y / 2,
                                         cubePosition.z + cubeSize.z / 2}});
        if (collision.hit == 1) {
          isOpenPanel = !isOpenPanel;
          collision.hit = false;
        }
      }
    }

    //--- Draw ---
    BeginDrawing();
    ClearBackground(BLACK);

    // --- 3D Draw ---
    BeginMode3D(camera);
    DrawCube(cubePosition, cubeSize.x, cubeSize.y, cubeSize.z, RED);
    DrawCubeWires(cubePosition, cubeSize.x, cubeSize.y, cubeSize.z, MAROON);
    DrawGrid(10, 1.0f);
    EndMode3D();
    //----------------

    if (client.GetSensorReady() && isOpenPanel) {
      Vector2 screenPos = GetWorldToScreen(cubePosition, camera);
      Rectangle panel = {screenPos.x + 20.0f, screenPos.y, 250.0f, 140.0f};
      GuiPanel(panel, "Sensor Name");
      GuiLabel((Rectangle){panel.x, panel.y + 20.0f, 200.0f, 20.0f},
               sensors.at(0).GetIndication().c_str());
    }

    EndDrawing();
    //------------
  }

  client.Close();
  CloseWindow();
  return 0;
}