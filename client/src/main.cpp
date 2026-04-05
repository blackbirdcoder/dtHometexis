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
  float px = -4.60f;
  float py = 3.88f;
  float pz = -1.95f;
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
    //====
    // kitchen wall 1
    DrawCube((Vector3){-0.15, 2.0f, -10.0f}, 20.0f, 4.0f, 0.3f, GRAY);
    DrawCubeWires((Vector3){-0.15, 2.0f, -10.0f}, 20.0f, 4.0f, 0.3f, YELLOW);
    // kitchen wall 2
    DrawCube((Vector3){10.0f, 2.0f, -7.76}, 0.3f, 4.0f, 4.8f, GRAY);
    DrawCubeWires((Vector3){10.0f, 2.0f, -7.76}, 0.3f, 4.0f, 4.8f, YELLOW);
    //  kitchen window
    DrawModelEx(modelWindowClose, (Vector3){10.0f, 1.2f, -4.03},
                (Vector3){0.0f, 1.0f, 0.0f}, 270.0f,
                (Vector3){4.0f, 3.0f, 3.0f}, WHITE);
    // kitchen wall 3 (mini)
    DrawCube((Vector3){10.0f, 0.6f, -4.06f}, 0.3f, 1.2f, 2.6f, GRAY);
    DrawCubeWires((Vector3){10.0f, 0.6f, -4.06f}, 0.3f, 1.2f, 2.6f, YELLOW);
    // kitchen wall 4
    DrawCube((Vector3){10.0f, 2.0f, 1.47f}, 0.3f, 4.0f, 8.5f, GRAY);
    DrawCubeWires((Vector3){10.0f, 2.0f, 1.47f}, 0.3f, 4.0f, 8.5f, YELLOW);
    // hall door
    DrawModelEx(modelDoorClose, (Vector3){10.20f, 0.0f, 7.46f},
                (Vector3){0.0f, 1.0f, 0.0f}, 0.0f, (Vector3){2.0f, 1.9f, 2.0f},
                WHITE);
    // hall wall 1
    DrawCube((Vector3){10.0f, 2.0f, 8.78f}, 0.3f, 4.0f, 2.65f, GRAY);
    DrawCubeWires((Vector3){10.0f, 2.0f, 8.78f}, 0.3f, 4.0f, 2.65f, YELLOW);
    // hall wall 2
    DrawCube((Vector3){2.85, 2.0f, 9.97f}, 14.0f, 4.0f, 0.3f, GRAY);
    DrawCubeWires((Vector3){2.85, 2.0f, 9.97f}, 14.0f, 4.0f, 0.3f, YELLOW);
    // bedroom window
    DrawModelEx(modelWindowClose, (Vector3){-5.4f, 1.2f, 9.99},
                (Vector3){0.0f, 1.0f, 0.0f}, 180.0f,
                (Vector3){4.0f, 3.0f, 3.0f}, WHITE);
    // bedroom wall 3 (mini)
    DrawCube((Vector3){-5.4f, 0.6f, 9.99f}, 2.51f, 1.2f, 0.3f, GRAY);
    DrawCubeWires((Vector3){-5.4f, 0.6f, 9.99f}, 2.51f, 1.2f, 0.3f, YELLOW);
    // bedroom wall 4
    DrawCube((Vector3){-8.37f, 2.0f, 9.97f}, 3.55f, 4.0f, 0.3f, GRAY);
    DrawCubeWires((Vector3){-8.37f, 2.0f, 9.97f}, 3.55f, 4.0f, 0.3f, YELLOW);
    // bedroom wall 5
    DrawCube((Vector3){-10.0f, 2.0f, 0.0}, 0.3f, 4.0f, 19.66, GRAY);
    DrawCubeWires((Vector3){-10.0f, 2.0f, 0.0}, 0.3f, 4.0f, 19.66, YELLOW);
    // bedroom wall 6
    DrawCube((Vector3){-1.19f, 2.0f, 5.05f}, 0.3f, 4.0f, 9.55f, GRAY);
    DrawCubeWires((Vector3){-1.19f, 2.0f, 5.05f}, 0.3f, 4.0f, 9.55f, YELLOW);
    // bedroom wall 7
    DrawCube((Vector3){-1.79f, 2.0f, 0.131f}, 1.5f, 4.0f, 0.3f, GRAY);
    DrawCubeWires((Vector3){-1.79f, 2.0f, 0.131f}, 1.5f, 4.0f, 0.3f, YELLOW);
    // bedroom door
    DrawModelEx(modelDoorClose, (Vector3){-2.551f, 0.0f, 0.0f},
                (Vector3){0.0f, 1.0f, 0.0f}, 90.0f, (Vector3){2.0f, 1.9f, 2.0f},
                WHITE);
    // bedroom wall 8
    DrawCube((Vector3){-7.1f, 2.0f, 0.131f}, 5.54f, 4.0f, 0.3f, GRAY);
    DrawCubeWires((Vector3){-7.1f, 2.0f, 0.131f}, 5.54f, 4.0f, 0.3f, YELLOW);
    // bathroom wall 1
    DrawCube((Vector3){-4.78, 2.0f, -7.76}, 0.3f, 4.0f, 4.18f, GRAY);
    DrawCubeWires((Vector3){-4.78, 2.0f, -7.76}, 0.3f, 4.0f, 4.18f, YELLOW);
    // bathroom door
    DrawModelEx(modelDoorClose, (Vector3){-4.60, 0.0f, -3.89f},
                (Vector3){0.0f, 1.0f, 0.0f}, 0.0f, (Vector3){2.0f, 1.9f, 2.0f},
                WHITE);DrawCube((Vector3){-4.78, 2.0f, -1.95}, 0.3f, 4.0f, 3.88f, GRAY);
    // bathroom wall 2
    DrawCube((Vector3){-4.78, 2.0f, -1.95}, 0.3f, 4.0f, 3.88f, GRAY);
    DrawCubeWires((Vector3){-4.78, 2.0f, -1.95}, 0.3f, 4.0f, 3.88f, YELLOW);
    //====
    DrawGrid(20, 1.0f);
    EndMode3D();
    //----------------

    //--- GUI ---
    //====
    GuiSlider(Rectangle{100.0f, 360.0f, 150.0, 20.0f}, "X",
              TextFormat("%2.2f", px), &px, -20.0f, 20.0f);
    GuiSlider(Rectangle{100.0f, 390.0f, 150.0, 20.0f}, "Y",
              TextFormat("%2.2f", py), &py, -20.0f, 20.0f);
    GuiSlider(Rectangle{100.0f, 420.0f, 150.0, 20.0f}, "Z",
              TextFormat("%2.2f", pz), &pz, -20.0f, 20.0f);
    //====
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