#include <iostream>
#include <raylib.h>
#include <string>
#include "../external/httplib/httplib.h"
#include "../external/inih/INIReader.h"

int main() {
  INIReader reader("settings.ini");

  if (reader.ParseError() < 0) {
    std::cout << "Can't load 'settings.ini'\n";
    return 1;
  }

  std::string host = reader.Get("server", "host", "localhost");
  unsigned port = reader.GetInteger("server", "port", 5000);
  std::string path = reader.Get("server", "path", "/data");

  std::string response;
  std::cout << "Hello! Client!" << '\n';

  std::string url = "http://" + host + ":" + std::to_string(port);
  httplib::Client cli(url);
  if (auto res = cli.Get(path)) {
    std::cout << "[*] Status: " << res->status << "[*] Body: " << res->body
              << '\n';
    response = res->body;
  }

  InitWindow(1024, 768, "Client DT");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText(response.c_str(), 20, 100, 27, GREEN);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}