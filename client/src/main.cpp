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

int main() {
  INIReader reader(ClientDigitalTwin::CONFIG_FILE);

  //--- Camera
  Camera3D camera = {0};
  camera.position = (Vector3){10.0f, 10.0f, 10.0f};
  camera.target = (Vector3){0.0f, 0.0f, 0.0f};
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;
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

  std::vector<ClientDigitalTwin::Sensor> sensors;
  ClientDigitalTwin::Client client(url, ClientDigitalTwin::PING_INTERVAL);
  client.Handler(sensors);
  client.Run();

  // --- Window App ---
  InitWindow(1024, 768, "Client Digital House");
  SetTargetFPS(60);
  GuiLoadStyle("assets/style/style_terminal.rgs");
  GuiSetStyle(DEFAULT, TEXT_SIZE, ClientDigitalTwin::SIZE);
  Model modelSensor = LoadModel("assets/models/sensor.glb");

  std::string t = ClientDigitalTwin::TAGS[ClientDigitalTwin::Tag::SENSOR];
  std::string m = "GetDataSensors";
  std::vector<std::string> v = {"parA", "parB"};
  ClientDigitalTwin::id_measure id = 2;
  client.Send(m, v, t, id);

  while (!WindowShouldClose()) {
    // --- Update ---
    bool isBusyCursorUI = false;

    if (client.IsSensorsReady()) {
      for (auto &sensor : sensors) {
        if (sensor.IsOpenWindow() &&
            CheckCollisionPointRec(GetMousePosition(),
                                   sensor.GetWindowRect())) {
          isBusyCursorUI = true;
          break;
        }
      }

      for (auto &sensor : sensors) {
        sensor.ClickHandler(camera, isBusyCursorUI);
      }
    }

    // ==== temporary implementation!!!
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
    //===========================
    //----------------

    //--- Draw ---
    BeginDrawing();
    ClearBackground(BLACK);

    // --- 3D Draw ---
    BeginMode3D(camera);
    if (client.IsSensorsReady()) {
      for (auto &sensor : sensors) {
        sensor.Draw(modelSensor);
      }
    }
    DrawGrid(20, 1.0f);
    EndMode3D();
    //----------------

    //--- GUI ---
    if (client.IsSensorsReady()) {
      for (auto &sensor : sensors) {
        if (sensor.IsOpenWindow()) {
          sensor.ShowWindow(camera);
        }
      }
    }
    //---------------

    EndDrawing();
    //----------------
  }
  UnloadModel(modelSensor);
  client.Close();
  CloseWindow();
  //----------------

  return 0;
}