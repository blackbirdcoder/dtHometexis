#include "server.hpp"
#include <iostream>
#include <nlohmann/json.hpp>
#include <random>

ServerDigitalTwin::Server::Server(const URL &url)
    : webSocket(url.port, url.host) {
  this->sensors = nlohmann::json::array();
  this->rooms =
      nlohmann::json::array({"kitchen", "bathroom", "hall", "bedroom"});
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
              data["sensors"] = generateSensors("kitchen");
              data["sensors"] = generateSensors("bathroom");
              data["sensors"] = generateSensors("hall");
              data["sensors"] = generateSensors("bedroom");

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
                       request["method"] == "GetNameRooms") {
              nlohmann::json data;
              data = this->rooms;
              response["id"] = request["id"];
              response["tag"] = request["tag"];
              response["result"] = data;
              ws.sendText(nlohmann::to_string(response));

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
            } else if (request.contains("method") &&
                       request["method"] == "GetHomeWallSolid") {
              response["id"] = request["id"];
              response["tag"] = request["tag"];
              response["result"] = generateWallSolid();
              ws.sendText(nlohmann::to_string(response));

            } else if (request.contains("method") &&
                       request["method"] == "GetHomeWindowsDoorsOpening") {
              std::cout << "*(SERVER)* GET WINDOW DOOR\n";

              response["id"] = request["id"];
              response["tag"] = request["tag"];
              response["result"] = generateWindowDoor();

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

nlohmann::json
ServerDigitalTwin::Server::generateSensors(const std::string &room) {
  float posY = 2.5f;
  if (room == "kitchen") {
    this->sensors.push_back({{"name", "kitchen_temp_1"},
                             {"type", "temperature"},
                             {"unit", "'C"},
                             {"value", randomValue(-50.0f, 50.0f)},
                             {"position", nlohmann::json({
                                              {"x", -2.0f},
                                              {"y", posY},
                                              {"z", -9.5f},
                                          })},
                             {"angle", 0.0f},
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
                                              {"x", -1.0f},
                                              {"y", posY},
                                              {"z", -9.5f},
                                          })},
                             {"angle", 0.0f},
                             {"options", nlohmann::json({
                                             {"humify", -1.0f},
                                             {"max", 65.0f},
                                             {"min", 35.0f},
                                             {"recommended", 45.0f},
                                         })}});
    this->sensors.push_back({{"name", "kitchen_gas_1"},
                             {"type", "gas"},
                             {"unit", "bool"},
                             {"value", 1.0f},
                             {"position", nlohmann::json({
                                              {"x", 0.0f},
                                              {"y", posY},
                                              {"z", -9.5f},
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
                                              {"x", 1.0},
                                              {"y", posY},
                                              {"z", -9.5},
                                          })},
                             {"angle", 0.0f},
                             {"options", nlohmann::json({
                                             {"limit", 50.0f},
                                         })}});
    this->sensors.push_back({{"name", "kitchen_light_1"},
                             {"type", "light"},
                             {"unit", "bool"},
                             {"value", 1.0f},
                             {"position", nlohmann::json({
                                              {"x", 2.0f},
                                              {"y", posY},
                                              {"z", -9.5},
                                          })},
                             {"angle", 0.0f},
                             {"options", nlohmann::json({
                                             {"bright", 50.0f},
                                         })}});
    this->sensors.push_back({{"name", "kitchen_motion_1"},
                             {"type", "motion"},
                             {"unit", "bool"},
                             {"value", 0.0f},
                             {"position", nlohmann::json({
                                              {"x", 3.0},
                                              {"y", posY},
                                              {"z", -9.5},
                                          })},
                             {"angle", 0.0f},
                             {"options", nlohmann::json({
                                             {"track", 1.0f},
                                             {"alarm", 0.0f},
                                         })}});
    this->sensors.push_back({{"name", "kitchen_window_1"},
                             {"type", "window"},
                             {"unit", "bool"},
                             {"value", 0.0f},
                             {"position", nlohmann::json({
                                              {"x", 4.0},
                                              {"y", posY},
                                              {"z", -9.5},
                                          })},
                             {"angle", 0.0f},
                             {"options", nlohmann::json({
                                             {"alarm", 0.0f},
                                         })}});
  } else if (room == "bathroom") {
    this->sensors.push_back({{"name", "bathroom_temp_1"},
                             {"type", "temperature"},
                             {"unit", "C"},
                             {"value", randomValue(-50.0f, 50.0f)},
                             {"position", nlohmann::json({
                                              {"x", -9.5f},
                                              {"y", posY},
                                              {"z", -7.0f},
                                          })},
                             {"angle", 90.0f},
                             {"options", nlohmann::json({
                                             {"heating", 1.0f},
                                             {"max", 35.0f},
                                             {"min", 3.0f},
                                             {"recommended", 21.0f},
                                         })}});
    this->sensors.push_back({{"name", "bathroom_hum_1"},
                             {"type", "humidity"},
                             {"unit", "%"},
                             {"value", randomValue(0.0f, 100.0f)},
                             {"position", nlohmann::json({
                                              {"x", -9.5f},
                                              {"y", posY},
                                              {"z", -6.0f},
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
                             {"value", 0.0f},
                             {"position", nlohmann::json({
                                              {"x", -9.5},
                                              {"y", posY},
                                              {"z", -5.0},
                                          })},
                             {"angle", 90.0f},
                             {"options", nlohmann::json({
                                             {"track", 1.0f},
                                             {"alarm", 0.0f},
                                         })}});
    this->sensors.push_back({{"name", "bathroom_door_1"},
                             {"type", "door"},
                             {"unit", "bool"},
                             {"value", 0.0f},
                             {"position", nlohmann::json({
                                              {"x", -9.5f},
                                              {"y", posY},
                                              {"z", -4.0f},
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
                                              {"x", -9.5f},
                                              {"y", posY},
                                              {"z", -3.0f},
                                          })},
                             {"angle", 90.0f},
                             {"options", nlohmann::json({
                                             {"bright", 50.0f},
                                         })}});
  } else if (room == "hall") {
    this->sensors.push_back({{"name", "hall_motion_1"},
                             {"type", "motion"},
                             {"unit", "bool"},
                             {"value", 0.0f},
                             {"position", nlohmann::json({
                                              {"x", -0.5f},
                                              {"y", posY},
                                              {"z", 8.0f},
                                          })},
                             {"angle", 90.0f},
                             {"options", nlohmann::json({
                                             {"track", 1.0f},
                                             {"alarm", 0.0f},
                                         })}});
    this->sensors.push_back({{"name", "hall_door_1"},
                             {"type", "door"},
                             {"unit", "bool"},
                             {"value", 0.0f},
                             {"position", nlohmann::json({
                                              {"x", -0.5f},
                                              {"y", posY},
                                              {"z", 7.0f},
                                          })},
                             {"angle", 90.0f},
                             {"options", nlohmann::json({
                                             {"alarm", 0.0f},
                                         })}});
    this->sensors.push_back({{"name", "hall_light_1"},
                             {"type", "light"},
                             {"unit", "bool"},
                             {"value", 1.0f},
                             {"position", nlohmann::json({
                                              {"x", -0.5f},
                                              {"y", posY},
                                              {"z", 6.0f},
                                          })},
                             {"angle", 90.0f},
                             {"options", nlohmann::json({
                                             {"bright", 50.0f},
                                         })}});
  } else if (room == "bedroom") {
    this->sensors.push_back({{"name", "bedroom_temp_1"},
                             {"type", "temperature"},
                             {"unit", "C"},
                             {"value", randomValue(-50.0f, 50.0f)},
                             {"position", nlohmann::json({
                                              {"x", -9.5f},
                                              {"y", posY},
                                              {"z", 9.0f},
                                          })},
                             {"angle", 90.0f},
                             {"options", nlohmann::json({
                                             {"heating", 1.0f},
                                             {"max", 35.0f},
                                             {"min", 5.0f},
                                             {"recommended", 21.0f},
                                         })}});
    this->sensors.push_back({{"name", "bedroom_motion_1"},
                             {"type", "motion"},
                             {"unit", "bool"},
                             {"value", 0.0f},
                             {"position", nlohmann::json({
                                              {"x", -9.5f},
                                              {"y", posY},
                                              {"z", 8.0},
                                          })},
                             {"angle", 90.0f},
                             {"options", nlohmann::json({
                                             {"track", 1.0f},
                                             {"alarm", 0.0f},
                                         })}});
    this->sensors.push_back({{"name", "bedroom_light_1"},
                             {"type", "light"},
                             {"unit", "bool"},
                             {"value", 1.0f},
                             {"position", nlohmann::json({
                                              {"x", -9.5f},
                                              {"y", posY},
                                              {"z", 7.0f},
                                          })},
                             {"angle", 90.0f},
                             {"options", nlohmann::json({
                                             {"bright", 50.0f},
                                         })}});
    this->sensors.push_back({{"name", "bedroom_hum_1"},
                             {"type", "humidity"},
                             {"unit", "%"},
                             {"value", randomValue(0.0f, 100.0f)},
                             {"position", nlohmann::json({
                                              {"x", -9.5f},
                                              {"y", posY},
                                              {"z", 6.0f},
                                          })},
                             {"angle", 90.0f},
                             {"options", nlohmann::json({
                                             {"humify", 1.0f},
                                             {"max", 65.0f},
                                             {"min", 35.0f},
                                             {"recommended", 45.0f},
                                         })}});
    this->sensors.push_back({{"name", "bedroom_door_1"},
                             {"type", "door"},
                             {"unit", "bool"},
                             {"value", 0.0f},
                             {"position", nlohmann::json({
                                              {"x", -9.5f},
                                              {"y", posY},
                                              {"z", 5.0f},
                                          })},
                             {"angle", 90.0f},
                             {"options", nlohmann::json({
                                             {"alarm", 0.0f},
                                         })}});
    this->sensors.push_back({{"name", "bedroom_window_1"},
                             {"type", "window"},
                             {"unit", "bool"},
                             {"value", 0.0f},
                             {"position", nlohmann::json({
                                              {"x", -9.5f},
                                              {"y", posY},
                                              {"z", 4.0},
                                          })},
                             {"angle", 90.0f},
                             {"options", nlohmann::json({
                                             {"alarm", 0.0f},
                                         })}});
  }
  return this->sensors;
}

nlohmann::json ServerDigitalTwin::Server::generateWallSolid() {
  nlohmann::json data = {

      // kitchen wall 1
      {{"type", "wall"},
       {"position", {-0.15, 2.0, -10.0}},
       {"scale", {20.0, 4.0, 0.3}}},

      // kitchen wall 2
      {{"type", "wall"},
       {"position", {10.0, 2.0, -7.76}},
       {"scale", {0.3, 4.0, 4.8}}},

      // kitchen wall 3 (mini)
      {{"type", "wall"},
       {"position", {10.0, 0.6, -4.06}},
       {"scale", {0.3, 1.2, 2.6}}},

      // kitchen wall 4
      {{"type", "wall"},
       {"position", {10.0, 2.0, 1.47}},
       {"scale", {0.3, 4.0, 8.5}}},

      // hall wall 1
      {{"type", "wall"},
       {"position", {10.0, 2.0, 8.78}},
       {"scale", {0.3, 4.0, 2.65}}},

      // hall wall 2
      {{"type", "wall"},
       {"position", {2.85, 2.0, 9.97}},
       {"scale", {14.0, 4.0, 0.3}}},

      // bedroom wall 1 (mini)
      {{"type", "wall"},
       {"position", {-5.4, 0.6, 9.99}},
       {"scale", {2.51, 1.2, 0.3}}},

      // bedroom wall 2
      {{"type", "wall"},
       {"position", {-8.37, 2.0, 9.97}},
       {"scale", {3.55, 4.0, 0.3}}},

      // bedroom wall 3
      {{"type", "wall"},
       {"position", {-10.0, 2.0, 0.0}},
       {"scale", {0.3, 4.0, 19.66}}},

      // bedroom wall 4
      {{"type", "wall"},
       {"position", {-1.19, 2.0, 5.05}},
       {"scale", {0.3, 4.0, 9.55}}},

      // bedroom wall 5
      {{"type", "wall"},
       {"position", {-1.79, 2.0, 0.131}},
       {"scale", {1.5, 4.0, 0.3}}},

      // bedroom wall 6
      {{"type", "wall"},
       {"position", {-7.1, 2.0, 0.131}},
       {"scale", {5.54, 4.0, 0.3}}},

      // bathroom wall 1
      {{"type", "wall"},
       {"position", {-4.78, 2.0, -7.76}},
       {"scale", {0.3, 4.0, 4.18}}},

      // bathroom wall 2
      {{"type", "wall"},
       {"position", {-4.78, 2.0, -1.95}},
       {"scale", {0.3, 4.0, 3.88}}}

  };

  return data;
}

nlohmann::json ServerDigitalTwin::Server::generateWindowDoor() {
  nlohmann::json data = {

      // Windows
      {{"type", "window"},
       {"position", {10.0f, 1.2f, -4.03f}}, // 10.0f
       {"rotation", 270.0f},
       {"scale", {4.0f, 3.0f, 3.0f}},
       {"room", "kitchen_window_1"}},

      {{"type", "window"},
       {"position", {-5.4f, 1.2f, 9.99f}},
       {"rotation", 180.0f},
       {"scale", {4.0f, 3.0f, 3.0f}},
       {"room", "bedroom_window_1"}},

      // Doors
      {{"type", "door"},
       {"position", {10.20f, 0.0f, 7.46f}},
       {"rotation", 0.0f},
       {"scale", {2.0f, 1.9f, 2.0f}},
       {"room", "hall_door_1"}},

      {{"type", "door"},
       {"position", {-2.551f, 0.0f, 0.0f}},
       {"rotation", 90.0f},
       {"scale", {2.0f, 1.9f, 2.0f}},
       {"room", "bedroom_door_1"}},

      {{"type", "door"},
       {"position", {-4.60f, 0.0f, -3.89f}},
       {"rotation", 0.0f},
       {"scale", {2.0f, 1.9f, 2.0f}},
       {"room", "bathroom_door_1"}}};

  return data;
}

float ServerDigitalTwin::Server::randomValue(float min, float max) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dist(min, max);
  return dist(gen);
}