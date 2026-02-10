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
  double randomValue(double min, double max); 
  ix::WebSocketServer webSocket;
};

} // namespace ServerDigitalTwin