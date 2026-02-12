#include "server.hpp"
#include <iostream>
#include <nlohmann/json.hpp>
#include <random>

ServerDigitalTwin::Server::Server(const URL &url)
    : webSocket(url.port, url.host) {}

void ServerDigitalTwin::Server::Handler() {

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
              data["kitchen"]["sensors"] = generate("kitchen");

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

nlohmann::json ServerDigitalTwin::Server::generate(const std::string &room) {
  nlohmann::json sensor = nlohmann::json::array();
  if (room == "kitchen") {
    sensor.push_back({
        {"name", "kitchen_temp_1"},
        {"type", "temperature"},
        {"unit", "'C"},
        {"value", randomValue(-50.0, 50.0)},
    });
    sensor.push_back({
        {"name", "kitchen_hum_1"},
        {"type", "humidity"},
        {"unit", "%"},
        {"value", randomValue(0.0, 100.0)},
    });
    sensor.push_back({
        {"name", "kitchen_gas_1"},
        {"type", "gas"},
        {"unit", "bool"},
        {"value", 0.0},
    });
    sensor.push_back({
        {"name", "kitchen_smoke_1"},
        {"type", "smoke"},
        {"unit", "bool"},
        {"value", 0.0},
    });
    sensor.push_back({
        {"name", "kitchen_light_1"},
        {"type", "light"},
        {"unit", "bool"},
        {"value", 1.0},
    });
    sensor.push_back({
        {"name", "kitchen_motion_1"},
        {"type", "motion"},
        {"unit", "bool"},
        {"value", 0.0},
    });
  }
  return sensor;
}

double ServerDigitalTwin::Server::randomValue(double min, double max) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> dist(min, max);
  return dist(gen);
}