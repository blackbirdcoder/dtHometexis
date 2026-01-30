#include <iostream>
#include "../external/inih/INIReader.h"
#include <string>
#include "settings.h"
#include <ixwebsocket/IXWebSocketServer.h>

const std::string ServerDigitalTwin::CONFIG_FILE = "settings.ini";

int main() {
  INIReader reader(ServerDigitalTwin::CONFIG_FILE);

  if (reader.ParseError() < 0) {
    std::cerr << "[!] Error: Can't load "
              << "'" << ServerDigitalTwin::CONFIG_FILE << "'\n";
    return 1;
  }

  ServerDigitalTwin::URL url = {
      reader.Get("server", "host", "0.0.0.0"),
      reader.GetInteger("server", "port", 5000),
      reader.Get("server", "data_path", "/data"),
  };

  ix::WebSocketServer server(url.port, url.host);
  server.setOnClientMessageCallback(
      [](std::shared_ptr<ix::ConnectionState> connection,
         ix::WebSocket &webSocket, const ix::WebSocketMessagePtr &msg) {
        if (msg->type == ix::WebSocketMessageType::Open) {
          webSocket.sendText("Hello client!");
        } else if (msg->type == ix::WebSocketMessageType::Message) {
          std::cout << "Received: " << msg->str << std::endl;
          webSocket.sendText("Echo: " + msg->str);
        } else if (msg->type == ix::WebSocketMessageType::Close) {
          std::cout << "Client disconnected\n";
        }
      });

  auto res = server.listen();
  if (!res.first) {
    std::cerr << "[!] Error: " << res.second << '\n';
    return 1;
  }

  server.start();
  std::cout << "[*] WebSocket server start ws://" << url.host << ":" << url.port
            << '\n';

  std::this_thread::sleep_for(std::chrono::hours(24));

  return 0;
}