#include "server.hpp"
#include <iostream>
#include <nlohmann/json.hpp>
#include "generator.hpp"

ServerDigitalTwin::Server::Server(const URL &url)
    : webSocket(url.port, url.host) {}

void ServerDigitalTwin::Server::Handler() {
  Generator generator;

  webSocket.setOnClientMessageCallback(
      [&](std::shared_ptr<ix::ConnectionState> connect, ix::WebSocket &ws,
          const ix::WebSocketMessagePtr &msg) {
        if (msg->type == ix::WebSocketMessageType::Open) {
          std::cout << "[*] Connection is open\n"
                    << "Remote IP: " << connect->getRemoteIp() << '\n'
                    << "Connect ID: " << connect->getId() << '\n'
                    << "Uri: " << msg->openInfo.uri << '\n';

          std::cout << "[*] Headers\n";
          for (auto it : msg->openInfo.headers) {
            std::cout << it.first << ": " << it.second << '\n';
          }

        } else if (msg->type == ix::WebSocketMessageType::Message) {
          nlohmann::json response;
          response["jsonrpc"] = "2.0";
          try {
            nlohmann::json request = nlohmann::json::parse(msg->str);
            std::cout << "[****] REQ:" << request << '\n';
            if (!request.contains("method")) {
              nlohmann::json error;
              error["code"] = -32602;
              error["message"] = "Invalid params";
              response["id"] = nlohmann::detail::value_t::null;
              response["tag"] = nlohmann::detail::value_t::null;
              response["error"] = error;
              ws.sendText(nlohmann::to_string(response));
            }

            if (request.contains("method") &&
                request["method"] == "GetDataSensors") {

              nlohmann::json data;
              data["kitchen"]["sensors"] = generator.Generate("kitchen");

              response["id"] = request["id"];
              response["tag"] = request["tag"];
              response["result"] = data;
              ws.sendText(nlohmann::to_string(response));

            } else if (request.contains("method")) {
              nlohmann::json error;
              error["code"] = -32601;
              error["message"] = "Method not found";
              response["id"] = nlohmann::detail::value_t::null;
              response["tag"] = nlohmann::detail::value_t::null;
              response["error"] = error;
              ws.sendText(nlohmann::to_string(response));
            }

          } catch (...) {
            nlohmann::json error;
            error["code"] = -32700;
            error["message"] = "Parse error";
            response["id"] = nlohmann::detail::value_t::null;
            response["tag"] = nlohmann::detail::value_t::null;
            response["error"] = error;
            ws.sendText(nlohmann::to_string(response));
          }

        } else if (msg->type == ix::WebSocketMessageType::Close) {
          std::cout << "[*] Connect id:" << connect->getId()
                    << " disconnected\n";
        }
      });
}

void ServerDigitalTwin::Server::Run() {
  auto res = webSocket.listen();
  if (!res.first) {
    throw std::runtime_error(
        "[!] Error: The server does not start, check the configuration");
  }

  webSocket.start();
  std::cout << "[*] WebSocket server start ws://" << webSocket.getHost() << ":"
            << webSocket.getPort() << '\n';
  webSocket.wait();
}