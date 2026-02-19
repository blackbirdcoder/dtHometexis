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
#include "camera.hpp"

int main() {
  INIReader reader(ClientDigitalTwin::CONFIG_FILE);

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
  ClientDigitalTwin::Camera camera3d;
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
    camera3d.Handler();
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
        sensor.ClickHandler(camera3d.Get(), isBusyCursorUI);
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
    BeginMode3D(camera3d.Get());
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
        sensor.DrawName(camera3d.Get(), camera3d.GetDistance());
      }

      for (auto &sensor : sensors) {
        if (sensor.IsOpenWindow()) {
          sensor.ShowWindow(camera3d.Get());
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