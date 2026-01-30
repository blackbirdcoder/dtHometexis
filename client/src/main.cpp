#include <iostream>
#include <raylib.h>
#include <string>
#include "../external/inih/INIReader.h"
#include <ixwebsocket/IXWebSocket.h>
#include <mutex>

int main() {
  std::mutex mut;
  INIReader reader("settings.ini");

  if (reader.ParseError() < 0) {
    std::cout << "Can't load 'settings.ini'\n";
    return 1;
  }

  std::string host = reader.Get("server", "host", "0.0.0.0");
  unsigned port = reader.GetInteger("server", "port", 5000);
  std::string path = reader.Get("server", "path", "/data");

  ix::WebSocket webSocket;
  std::string url("ws://" + host + ":" + std::to_string(port));
  webSocket.setUrl(url);
  webSocket.setPingInterval(45);

  std::string response;
  webSocket.setOnMessageCallback([&](const ix::WebSocketMessagePtr &msg) {
    if (msg->type == ix::WebSocketMessageType::Message) {
      mut.lock();
      response = msg->str;
      mut.unlock();
    }
  });

  webSocket.start();

  InitWindow(1024, 768, "Client Digital House");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText(response.c_str(), 20, 100, 27, YELLOW);
    EndDrawing();
  }

  CloseWindow();
  webSocket.stop();
  return 0;
}