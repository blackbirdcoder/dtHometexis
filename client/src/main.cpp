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
#include "storage.hpp"

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

  ClientDigitalTwin::Manager manager;
  std::vector<std::unique_ptr<ClientDigitalTwin::Sensor>> sensors;
  ClientDigitalTwin::Client client(url, ClientDigitalTwin::PING_INTERVAL);
  client.Handler(sensors, manager.GetMode());
  client.Run();

  // --- DB init ---
  std::string nameHome = reader.Get("client", "home", "Home");
  ClientDigitalTwin::Storage::Instance().Init();
  int homeID = ClientDigitalTwin::Storage::Instance().CreateHome(nameHome);
  //----------------

  // --- Window App ---
  InitWindow(1024, 768, "Digital Twin Hometexis");
  SetTargetFPS(60);
  ClientDigitalTwin::Camera camera3d;
  GuiLoadStyle("assets/style/style_terminal.rgs");
  GuiSetStyle(DEFAULT, TEXT_SIZE, ClientDigitalTwin::SIZE);
  Model modelSensor = LoadModel("assets/models/sensor.glb");
  Model modelWindowOpen = LoadModel("assets/models/windowOpen.glb");
  Model modelWindowClose = LoadModel("assets/models/windowClose.glb");
  Model modelDoorClose = LoadModel("assets/models/doorClose.glb");
  Model modelDoorOpen = LoadModel("assets/models/doorOpen.glb");
  client.Send("GetDataSensors", {},
              ClientDigitalTwin::TAGS[ClientDigitalTwin::Tag::SENSOR],
              manager.CountRequestID());
  client.Send("GetNameRooms", {},
              ClientDigitalTwin::TAGS[ClientDigitalTwin::Tag::ROOM],
              manager.CountRequestID());
  client.Send("GetHomeWallSolid", {},
              ClientDigitalTwin::TAGS[ClientDigitalTwin::Tag::HOME_WALL_SOLID],
              manager.CountRequestID());
  client.Send("GetHomeWindowsDoorsOpening", {},
              ClientDigitalTwin::TAGS[ClientDigitalTwin::Tag::HOME_OPENING],
              manager.CountRequestID());

  while (!WindowShouldClose()) {
    // --- Update ---
    if (client.IsNameRoomsReady() && !manager.IsCreateTableDB()) {
      for (const auto room : client.GetNameRooms()) {
        int id =
            ClientDigitalTwin::Storage::Instance().CreateRoom(homeID, room, 1);
        manager.MakeRoomIdentification(id, 1, room);
      }
      manager.EnableTableCreation();
    }

    camera3d.Handler();
    bool isBusyCursorUI = false;

    if (client.IsSensorsReady()) {

      if (!manager.IsCreateSensorDB()) {
        for (auto &sensor : sensors) {
          std::string type = sensor->GetType();
          if (type == "Temperature" || type == "Humidity") {
            std::string room = sensor->GetWhichRoom();
            int id = manager.GetRoomID(room);
            if (id != -1) {
              manager.AddSensorID(
                  sensor.get(),
                  ClientDigitalTwin::Storage::Instance().CreateSensor(id,
                                                                      type));
            }
          }
        }
        manager.EnableSensorCreation();
      }

      if (manager.GetMode() == ClientDigitalTwin::Mode::CONTROL &&
          client.IsAllowUpdate()) {
        client.Send("UpdateDataSensors", {},
                    ClientDigitalTwin::TAGS[ClientDigitalTwin::Tag::UPDATE],
                    manager.CountRequestID());

        if (!manager.IsSensorDataAdded()) {
          for (auto &sensor : sensors) {
            auto it = manager.GetSensorsID().find(sensor.get());
            if (it != manager.GetSensorsID().end()) {
              ClientDigitalTwin::Storage::Instance().AddSensorData(
                  it->second, sensor->GetValue());
            }
          }
          manager.EnableSensorDataAdded();
        }

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
                      ClientDigitalTwin::TAGS[ClientDigitalTwin::Tag::SET],
                      manager.CountRequestID());
          client.Send("UpdateDataSensors", {},
                      ClientDigitalTwin::TAGS[ClientDigitalTwin::Tag::UPDATE],
                      manager.CountRequestID());
          sensor->ResetChangeFlagOption();
        }

        if (sensor->IsChangeValue()) {
          std::vector<std::string> params{
              nlohmann::json(sensor->GetName()),
              nlohmann::to_string(nlohmann::json(sensor->GetNewValue()))};
          client.Send("SetSensorValue", params,
                      ClientDigitalTwin::TAGS[ClientDigitalTwin::Tag::SET],
                      manager.CountRequestID());
          client.Send("UpdateDataSensors", {},
                      ClientDigitalTwin::TAGS[ClientDigitalTwin::Tag::UPDATE],
                      manager.CountRequestID());
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

    if (client.IsWallsReady()) {
      for (const auto &obj : client.GetWalls()) {
        auto position = obj["position"];
        auto scale = obj["scale"];
        Vector3 positions = {position[0], position[1], position[2]};
        Vector3 size = {scale[0], scale[1], scale[2]};
        DrawCube(positions, size.x, size.y, size.z, GRAY);
        DrawCubeWires(positions, size.x, size.y, size.z, YELLOW);
      }
    }

    if (client.IsOpeningReady()) {
      for (const auto &obj : client.GetOpening()) {
        std::string type = obj["type"];
        Vector3 position = {obj["position"][0], obj["position"][1],
                            obj["position"][2]};
        Vector3 scale = {obj["scale"][0], obj["scale"][1], obj["scale"][2]};
        float rotation = obj.value("rotation", 0.0f);

        if (type == "window") {
          DrawModelEx(modelWindowClose, position, (Vector3){0.0f, 1.0f, 0.0f},
                      rotation, scale, WHITE);
        } else if (type == "door") {
          DrawModelEx(modelDoorClose, position, (Vector3){0.0f, 1.0f, 0.0f},
                      rotation, scale, WHITE);
        }
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
  UnloadModel(modelWindowOpen);
  UnloadModel(modelWindowClose);
  UnloadModel(modelDoorClose);
  UnloadModel(modelDoorOpen);
  client.Close();
  CloseWindow();
  //----------------

  return 0;
}