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
#include <memory>
#include "manager.hpp"
#include <nlohmann/json.hpp>

int main() {
  bool check = false;

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

  ClientDigitalTwin::Manager manager;

  std::vector<std::unique_ptr<ClientDigitalTwin::Sensor>> sensors;
  ClientDigitalTwin::Client client(url, ClientDigitalTwin::PING_INTERVAL);
  client.Handler(sensors, manager.GetMode());
  client.Run();

  // --- Window App ---
  InitWindow(1024, 768, "Digital Twin Hometexis");
  SetTargetFPS(60);
  ClientDigitalTwin::Camera camera3d;
  GuiLoadStyle("assets/style/style_terminal.rgs");
  GuiSetStyle(DEFAULT, TEXT_SIZE, ClientDigitalTwin::SIZE);
  Model modelSensor = LoadModel("assets/models/sensor.glb");
  client.Send("GetDataSensors", {},
              ClientDigitalTwin::TAGS[ClientDigitalTwin::Tag::SENSOR], 2);

  while (!WindowShouldClose()) {
    // --- Update ---
    camera3d.Handler();
    bool isBusyCursorUI = false;

    if (client.IsSensorsReady()) {

      if (manager.GetMode() == ClientDigitalTwin::Mode::CONTROL &&
          client.IsAllowUpdate()) {
        client.Send("UpdateDataSensors", {},
                    ClientDigitalTwin::TAGS[ClientDigitalTwin::Tag::UPDATE], 3);
        client.SetAllowUpdate(false);

      } else if (manager.GetMode() == ClientDigitalTwin::Mode::SIMULATION) {
        client.SetAllowUpdate(true);
      }

      for (auto &sensor : sensors) {
        sensor->ModeUpdate(manager.GetMode());

        if (sensor->IsOpenWindow() &&
            CheckCollisionPointRec(GetMousePosition(),
                                   sensor->GetWindowRect())) {
          isBusyCursorUI = true;
          break;
        }
      }

      for (auto &sensor : sensors) {
        sensor->ClickHandler(camera3d.Get(), isBusyCursorUI);
      }

      for (auto &sensor : sensors) {
        if (sensor->IsChangeOptions()) {
          std::vector<std::string> params{
              nlohmann::json(sensor->GetName()),
              nlohmann::to_string(nlohmann::json(sensor->GetNewOption()))}; 
          client.Send("SetSensorOptions", params,
                      ClientDigitalTwin::TAGS[ClientDigitalTwin::Tag::SET], 4);
          client.Send("UpdateDataSensors", {},
                      ClientDigitalTwin::TAGS[ClientDigitalTwin::Tag::UPDATE],
                      5);
          sensor->ResetChangeFlagOption();
        }

        if (sensor->IsChangeValue()) {
          std::vector<std::string> params{
              nlohmann::json(sensor->GetName()),
              nlohmann::to_string(nlohmann::json(sensor->GetNewValue()))};
          client.Send("SetSensorValue", params,
                      ClientDigitalTwin::TAGS[ClientDigitalTwin::Tag::SET], 6);
          client.Send("UpdateDataSensors", {},
                      ClientDigitalTwin::TAGS[ClientDigitalTwin::Tag::UPDATE],
                      7);
          sensor->ResetChangeFlagValue();
        }
      }
    }
    //----------------

    //--- Draw ---
    BeginDrawing();
    ClearBackground(BLACK);
    // --- 3D Draw ---
    BeginMode3D(camera3d.Get());
    if (client.IsSensorsReady()) {
      for (auto &sensor : sensors) {
        sensor->Draw(modelSensor);
      }
    }
    DrawGrid(20, 1.0f);
    EndMode3D();
    //----------------

    //--- GUI ---
    manager.SelectOperatingMode();
    manager.DrawTextMode();
    if (client.IsSensorsReady()) {
      for (auto &sensor : sensors) {
        sensor->DrawName(camera3d.Get(), camera3d.GetDistance());
      }

      for (auto &sensor : sensors) {
        if (sensor->IsOpenWindow()) {
          sensor->ShowWindow(camera3d.Get());
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