#pragma once
#include "settings.h"
#include <ixwebsocket/IXWebSocketServer.h>
#include <nlohmann/json.hpp>

namespace ServerDigitalTwin {
class Server {
public:
  Server(const URL &url);
  void Handler();
  void Run();

private:
  nlohmann::json generateSensors(const std::string &room);
  nlohmann::json generateWallSolid();
  nlohmann::json generateWindowDoor();
  float randomValue(float min, float max);
  ix::WebSocketServer webSocket;
  nlohmann::json sensors;
  nlohmann::json rooms;
};

} // namespace ServerDigitalTwin