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
  nlohmann::json generate(const std::string &room);
  float randomValue(float min, float max);
  ix::WebSocketServer webSocket;
  nlohmann::json sensors;
  nlohmann::json rooms;
};

} // namespace ServerDigitalTwin