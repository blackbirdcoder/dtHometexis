#pragma once
#include "settings.h"
#include <ixwebsocket/IXWebSocketServer.h>


namespace ServerDigitalTwin {
class Server {
public:
  Server(const URL &url);
  void Handler();
  void Run();

private:
  ix::WebSocketServer webSocket;
};

} // namespace ServerDigitalTwin