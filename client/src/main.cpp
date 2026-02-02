#include <iostream>
#include <raylib.h>
#include <string>
#include <inih/INIReader.h>
#include <ixwebsocket/IXWebSocket.h>
#include <mutex>
#include "settings.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main() {
  std::string response = "Work app";
  std::mutex mut;
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

  ix::WebSocket webSocket;
  std::string schema = "ws://" + url.host + ":" + std::to_string(url.port);
  webSocket.setUrl(schema);
  webSocket.setPingInterval(45);

  webSocket.setOnMessageCallback([&](const ix::WebSocketMessagePtr &msg) {
    if (msg->type == ix::WebSocketMessageType::Message) {
      mut.lock();
      json j_complete = json::parse(msg->str);
      if (j_complete["status"] == 1) {
        response = j_complete["answer"];
      }
      if (j_complete["status"] == 2) {
        response = j_complete["answer"];
      }
      mut.unlock();
    }
  });

  webSocket.start();

  InitWindow(1024, 768, "Client Digital House");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_H)) {
      json j;
      j["command"] = "Hi";
      webSocket.sendText(nlohmann::to_string(j));
    }
    if (IsKeyPressed(KEY_B)) {
      json j;
      j["command"] = "Bye";
      webSocket.sendText(nlohmann::to_string(j));
    }

    BeginDrawing();
    ClearBackground(BLACK);
    DrawText(response.c_str(), 20, 100, 27, (Color){112, 31, 126, 255});
    EndDrawing();
  }

  CloseWindow();
  webSocket.stop();
  return 0;
}