#include "server.hpp"
#include <iostream>
#include <nlohmann/json.hpp>
#include <random>

ServerDigitalTwin::Server::Server(const URL &url)
    : webSocket(url.port, url.host) {
  this->sensors = nlohmann::json::array();
}

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
              data["sensors"] = generate("kitchen");
              data["sensors"] = generate("bathroom");

              response["id"] = request["id"];
              response["tag"] = request["tag"];
              response["result"] = data;
              ws.sendText(nlohmann::to_string(response));

            } else if (request.contains("method") &&
                       request["method"] == "UpdateDataSensors") {

              nlohmann::json data;
              data["sensors"] = this->sensors;

              response["id"] = request["id"];
              response["tag"] = request["tag"];
              response["result"] = data;
              ws.sendText(nlohmann::to_string(response));

            } else if (request.contains("method") &&
                       request["method"] == "SetSensorOptions") {

              std::string name = request["params"][0];
              auto options = nlohmann::json::parse(
                  request["params"][1].get<std::string>());

              for (auto &sensor : this->sensors) {
                if (sensor.at("name") == name) {
                  auto &senOpt = sensor.at("options");
                  for (const auto &opt : options.items()) {
                    if (senOpt.contains(opt.key())) {
                      senOpt[opt.key()] = opt.value();
                    }
                  }
                }
              }

            } else if (request.contains("method") &&
                       request["method"] == "SetSensorValue") {

              std::string name = request["params"][0];
              auto value = nlohmann::json::parse(
                  request["params"][1].get<std::string>());
              for (auto &sensor : this->sensors) {
                if (sensor.at("name") == name) {
                  if (sensor.contains("value") && value.contains("value")) {
                    std::cout << "*(SENSOR)* OLD VALUE: " << sensor["value"]
                              << '\n';
                    sensor["value"] = value["value"];
                    std::cout << "*(SENSOR)* NEW VALUE: " << sensor["value"]
                              << '\n';
                  }
                }
              }

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
          this->sensors.clear();
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
  float posY = 3.0f;
  if (room == "kitchen") {
    this->sensors.push_back({{"name", "kitchen_temp_1"},
                             {"type", "temperature"},
                             {"unit", "'C"},
                             {"value", randomValue(-50.0f, 50.0f)},
                             {"position", nlohmann::json({
                                              {"x", 2.0f},
                                              {"y", posY},
                                              {"z", 3.0f},
                                          })},
                             {"angle", 180.0f},
                             {"options", nlohmann::json({
                                             {"heating", 1.0f},
                                             {"max", 35.0f},
                                             {"min", 5.0f},
                                             {"recommended", 21.0f},
                                         })}});
    this->sensors.push_back({{"name", "kitchen_hum_1"},
                             {"type", "humidity"},
                             {"unit", "%"},
                             {"value", randomValue(0.0f, 100.0f)},
                             {"position", nlohmann::json({
                                              {"x", 3.0f},
                                              {"y", posY},
                                              {"z", 3.0f},
                                          })},
                             {"angle", 180.0f},
                             {"options", nlohmann::json({
                                             {"humify", 1.0f},
                                             {"max", 65.0f},
                                             {"min", 35.0f},
                                             {"recommended", 45.0f},
                                         })}});
    this->sensors.push_back({{"name", "kitchen_gas_1"},
                             {"type", "gas"},
                             {"unit", "bool"},
                             {"value", 1.0f},
                             {"position", nlohmann::json({
                                              {"x", 2.0},
                                              {"y", posY},
                                              {"z", 1.0},
                                          })},
                             {"angle", 0.0},
                             {"options", nlohmann::json({
                                             {"limit", 10.0f},
                                             {
                                                 "leak",
                                                 randomValue(0.0f, 100.0f),
                                             },
                                         })}});
    this->sensors.push_back({{"name", "kitchen_smoke_1"},
                             {"type", "smoke"},
                             {"unit", "%"},
                             {"value", randomValue(0.0f, 100.0f)},
                             {"position", nlohmann::json({
                                              {"x", 3.0},
                                              {"y", posY},
                                              {"z", 1.0},
                                          })},
                             {"angle", 0.0},
                             {"options", nlohmann::json({
                                             {"limit", 50.0f},
                                         })}});
    this->sensors.push_back({{"name", "kitchen_light_1"},
                             {"type", "light"},
                             {"unit", "bool"},
                             {"value", 1.0f},
                             {"position", nlohmann::json({
                                              {"x", 4.0f},
                                              {"y", posY},
                                              {"z", 1.0f},
                                          })},
                             {"angle", 0.0f},
                             {"options", nlohmann::json({
                                             {"bright", 50.0f},
                                         })}});
    this->sensors.push_back({{"name", "kitchen_motion_1"},
                             {"type", "motion"},
                             {"unit", "bool"},
                             {"value", 0.0},
                             {"position", nlohmann::json({
                                              {"x", 4.0},
                                              {"y", posY},
                                              {"z", 3.0},
                                          })},
                             {"angle", 180.0},
                             {"options", nlohmann::json({
                                             {"track", 1.0f},
                                             {"alarm", 0.0f},
                                         })}});
    this->sensors.push_back({{"name", "kitchen_door_1"},
                             {"type", "door"},
                             {"unit", "bool"},
                             {"value", 0.0f},
                             {"position", nlohmann::json({
                                              {"x", 5.0f},
                                              {"y", posY},
                                              {"z", 3.0f},
                                          })},
                             {"angle", 180.0f},
                             {"options", nlohmann::json({
                                             {"alarm", 0.0f},
                                         })}});
    this->sensors.push_back({{"name", "kitchen_window_1"},
                             {"type", "window"},
                             {"unit", "bool"},
                             {"value", 0.0},
                             {"position", nlohmann::json({
                                              {"x", 5.0},
                                              {"y", posY},
                                              {"z", 1.0},
                                          })},
                             {"angle", 0.0},
                             {"options", nlohmann::json({
                                             {"alarm", 0.0f},
                                         })}});
  } else if (room == "bathroom") {
    this->sensors.push_back({{"name", "bathroom_temp_1"},
                             {"type", "temperature"},
                             {"unit", "'C"},
                             {"value", randomValue(-50.0f, 50.0f)},
                             {"position", nlohmann::json({
                                              {"x", -4.0f},
                                              {"y", posY},
                                              {"z", 6.0f},
                                          })},
                             {"angle", 90.0f},
                             {"options", nlohmann::json({
                                             {"heating", 1.0f},
                                             {"max", 35.0f},
                                             {"min", 5.0f},
                                             {"recommended", 21.0f},
                                         })}});
    this->sensors.push_back({{"name", "bathroom_hum_1"},
                             {"type", "humidity"},
                             {"unit", "%"},
                             {"value", randomValue(0.0f, 100.0f)},
                             {"position", nlohmann::json({
                                              {"x", -4.0f},
                                              {"y", posY},
                                              {"z", 5.0f},
                                          })},
                             {"angle", 90.0f},
                             {"options", nlohmann::json({
                                             {"humify", 1.0f},
                                             {"max", 65.0f},
                                             {"min", 35.0f},
                                             {"recommended", 45.0f},
                                         })}});
    this->sensors.push_back({{"name", "bathroom_motion_1"},
                             {"type", "motion"},
                             {"unit", "bool"},
                             {"value", 0.0},
                             {"position", nlohmann::json({
                                              {"x", -4.0},
                                              {"y", posY},
                                              {"z", 4.0},
                                          })},
                             {"angle", 90.0},
                             {"options", nlohmann::json({
                                             {"track", 1.0f},
                                             {"alarm", 0.0f},
                                         })}});
    this->sensors.push_back({{"name", "bathroom_door_1"},
                             {"type", "door"},
                             {"unit", "bool"},
                             {"value", 0.0f},
                             {"position", nlohmann::json({
                                              {"x", -4.0f},
                                              {"y", posY},
                                              {"z", 3.0f},
                                          })},
                             {"angle", 90.0f},
                             {"options", nlohmann::json({
                                             {"alarm", 0.0f},
                                         })}});
    this->sensors.push_back({{"name", "bathroom_light_1"},
                             {"type", "light"},
                             {"unit", "bool"},
                             {"value", 1.0f},
                             {"position", nlohmann::json({
                                              {"x", -4.0f},
                                              {"y", posY},
                                              {"z", 2.0f},
                                          })},
                             {"angle", 90.0f},
                             {"options", nlohmann::json({
                                             {"bright", 50.0f},
                                         })}});
  }
  return this->sensors;
}

float ServerDigitalTwin::Server::randomValue(float min, float max) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dist(min, max);
  return dist(gen);
}