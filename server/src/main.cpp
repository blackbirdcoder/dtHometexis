#include <iostream>
#include <inih/INIReader.h>
#include <string>
#include "settings.h"
#include <ixwebsocket/IXWebSocketServer.h>
#include <nlohmann/json.hpp>
#include "server.hpp"

// using json = nlohmann::json;
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
      (uint16_t)reader.GetInteger("server", "port", 5000),
  };

  ServerDigitalTwin::Server server(url);
  server.Handler();
  server.Run();

  //--- OLD
  // ix::WebSocketServer server(url.port, url.host);
  // server.setOnClientMessageCallback(
  //     [&url](std::shared_ptr<ix::ConnectionState> connection,
  //            ix::WebSocket &webSocket, const ix::WebSocketMessagePtr &msg) {
  //       if (msg->type == ix::WebSocketMessageType::Open) {
  //         std::cout << "[*] Connect Info" << '\n';
  //         std::cout << "id: " << connection->getId() << std::endl;
  //         std::cout << "Uri: " << msg->openInfo.uri << std::endl;
  //         std::cout << "Headers:" << std::endl;
  //         for (auto it : msg->openInfo.headers) {
  //           std::cout << it.first << ": " << it.second << std::endl;
  //         }
  //       } else if (msg->type == ix::WebSocketMessageType::Message) {
  //         json command = json::parse(msg->str);
  //         if (command["command"] == "Hi") {
  //           json j;
  //           j["status"] = 1;
  //           j["answer"] = "Hello";
  //           webSocket.sendText(nlohmann::to_string(j));
  //         } else if (command["command"] == "Bye") {
  //           json j;
  //           j["status"] = 2;
  //           j["answer"] = "Goodbye";
  //           webSocket.sendText(nlohmann::to_string(j));
  //         }

  //       } else if (msg->type == ix::WebSocketMessageType::Close) {
  //         std::cout << "[*] Connect id:" << connection->getId()
  //                   << " disconnected" << '\n';
  //       }
  //     });

  // auto res = server.listen();
  // if (!res.first) {
  //   std::cerr << "[!] Error: " << res.second << '\n';
  //   return 1;
  // }

  // server.start();
  // std::cout << "[*] WebSocket server start ws://" << url.host << ":" <<
  // url.port
  //           << '\n';

  // server.wait();
  // std::this_thread::sleep_for(std::chrono::hours(24));

  return 0;
}