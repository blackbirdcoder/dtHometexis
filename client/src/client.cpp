#include "client.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <memory>
#include "sensors/temperature.hpp"
#include "sensors/humidity.hpp"
#include "sensors/gas.hpp"
#include "sensors/smoke.hpp"
#include "sensors/light.hpp"
#include "sensors/motion.hpp"
#include "sensors/door.hpp"
#include "sensors/window.hpp"

ClientDigitalTwin::Client::Client(const URL &url, const int ping) {
  std::string schema = "ws://" + url.host + ":" + std::to_string(url.port);
  this->webSocket.setUrl(schema);
  this->webSocket.setPingInterval(ping);
  this->isReadySensors = false;
  this->isParseError = false;
  this->isAllowUpdate = true;
  this->isReadyNameRooms = false;
}

void ClientDigitalTwin::Client::Handler(
    std::vector<std::unique_ptr<ClientDigitalTwin::Sensor>> &sensors,
    Mode mode) {
  this->webSocket.setOnMessageCallback([&sensors, mode, this](
                                           const ix::WebSocketMessagePtr &msg) {
    if (msg->type == ix::WebSocketMessageType::Message) {
      this->mut.lock();
      try {
        nlohmann::json response = nlohmann::json::parse(msg->str);
        if (response["tag"] ==
            ClientDigitalTwin::TAGS[ClientDigitalTwin::Tag::HOME]) {
          std::cout << "(*CLIENT*) IT IS HOME" << '\n';

        } else if (response["tag"] ==
                   ClientDigitalTwin::TAGS[ClientDigitalTwin::Tag::SENSOR]) {
          if (response["id"] != nlohmann::detail::value_t::null) {
            std::cout << "(*CLIENT*) RESPONSE RESULT: " << response["result"]
                      << "\n\n";
            auto data = response["result"]["sensors"];
            // std::cout << "(*CLIENT*) DATA: " << data << "\n";

            for (int i = 0; i < data.size(); ++i) {
              std::unique_ptr<Sensor> sensor;
              if (data[i]["type"].get<std::string>() == "temperature") {
                sensor = std::make_unique<Temperature>(
                    data[i]["name"].get<std::string>(),
                    data[i]["type"].get<std::string>(),
                    data[i]["unit"].get<std::string>(),
                    data[i]["value"].get<float>(),
                    (Vector3){data[i]["position"]["x"].get<float>(),
                              data[i]["position"]["y"].get<float>(),
                              data[i]["position"]["z"].get<float>()},
                    data[i]["angle"].get<float>(),
                    data[i]["options"].get<std::map<std::string, float>>(),
                    mode);

              } else if (data[i]["type"].get<std::string>() == "humidity") {
                sensor = std::make_unique<Humidity>(
                    data[i]["name"].get<std::string>(),
                    data[i]["type"].get<std::string>(),
                    data[i]["unit"].get<std::string>(),
                    data[i]["value"].get<float>(),
                    (Vector3){data[i]["position"]["x"].get<float>(),
                              data[i]["position"]["y"].get<float>(),
                              data[i]["position"]["z"].get<float>()},
                    data[i]["angle"].get<float>(),
                    data[i]["options"].get<std::map<std::string, float>>(),
                    mode);

              } else if (data[i]["type"].get<std::string>() == "gas") {
                sensor = std::make_unique<Gas>(
                    data[i]["name"].get<std::string>(),
                    data[i]["type"].get<std::string>(),
                    data[i]["unit"].get<std::string>(),
                    data[i]["value"].get<float>(),
                    (Vector3){data[i]["position"]["x"].get<float>(),
                              data[i]["position"]["y"].get<float>(),
                              data[i]["position"]["z"].get<float>()},
                    data[i]["angle"].get<float>(),
                    data[i]["options"].get<std::map<std::string, float>>(),
                    mode);

              } else if (data[i]["type"].get<std::string>() == "smoke") {
                sensor = std::make_unique<Smoke>(
                    data[i]["name"].get<std::string>(),
                    data[i]["type"].get<std::string>(),
                    data[i]["unit"].get<std::string>(),
                    data[i]["value"].get<float>(),
                    (Vector3){data[i]["position"]["x"].get<float>(),
                              data[i]["position"]["y"].get<float>(),
                              data[i]["position"]["z"].get<float>()},
                    data[i]["angle"].get<float>(),
                    data[i]["options"].get<std::map<std::string, float>>(),
                    mode);

              } else if (data[i]["type"].get<std::string>() == "light") {
                sensor = std::make_unique<Light>(
                    data[i]["name"].get<std::string>(),
                    data[i]["type"].get<std::string>(),
                    data[i]["unit"].get<std::string>(),
                    data[i]["value"].get<float>(),
                    (Vector3){data[i]["position"]["x"].get<float>(),
                              data[i]["position"]["y"].get<float>(),
                              data[i]["position"]["z"].get<float>()},
                    data[i]["angle"].get<float>(),
                    data[i]["options"].get<std::map<std::string, float>>(),
                    mode);

              } else if (data[i]["type"].get<std::string>() == "motion") {
                sensor = std::make_unique<Motion>(
                    data[i]["name"].get<std::string>(),
                    data[i]["type"].get<std::string>(),
                    data[i]["unit"].get<std::string>(),
                    data[i]["value"].get<float>(),
                    (Vector3){data[i]["position"]["x"].get<float>(),
                              data[i]["position"]["y"].get<float>(),
                              data[i]["position"]["z"].get<float>()},
                    data[i]["angle"].get<float>(),
                    data[i]["options"].get<std::map<std::string, float>>(),
                    mode);

              } else if (data[i]["type"].get<std::string>() == "door") {
                sensor = std::make_unique<Door>(
                    data[i]["name"].get<std::string>(),
                    data[i]["type"].get<std::string>(),
                    data[i]["unit"].get<std::string>(),
                    data[i]["value"].get<float>(),
                    (Vector3){data[i]["position"]["x"].get<float>(),
                              data[i]["position"]["y"].get<float>(),
                              data[i]["position"]["z"].get<float>()},
                    data[i]["angle"].get<float>(),
                    data[i]["options"].get<std::map<std::string, float>>(),
                    mode);

              } else if (data[i]["type"].get<std::string>() == "window") {
                sensor = std::make_unique<Window>(
                    data[i]["name"].get<std::string>(),
                    data[i]["type"].get<std::string>(),
                    data[i]["unit"].get<std::string>(),
                    data[i]["value"].get<float>(),
                    (Vector3){data[i]["position"]["x"].get<float>(),
                              data[i]["position"]["y"].get<float>(),
                              data[i]["position"]["z"].get<float>()},
                    data[i]["angle"].get<float>(),
                    data[i]["options"].get<std::map<std::string, float>>(),
                    mode);
              }

              if (sensor) {
                sensors.push_back(std::move(sensor));
              }
            }
            isReadySensors = true;
            std::cout << "(*CLIENT*) SENSOR SIZE: " << sensors.size() << "\n";
          }
        } else if (response["tag"] ==
                   ClientDigitalTwin::TAGS[ClientDigitalTwin::Tag::UPDATE]) {
          auto data = response["result"]["sensors"];
          std::cout << "(*CLIENT*) UPDATE BRANCH SIZE: " << data.size() << '\n';

          for (int i = 0; i < data.size(); ++i) {
            for (auto &sensor : sensors) {
              if (sensor->GetName() == data[i]["name"].get<std::string>()) {
                sensor->Update(
                    data[i]["value"].get<float>(),
                    data[i]["options"].get<std::map<std::string, float>>());
                break;
              }
            }
          }
        } else if (response["tag"] ==
                   ClientDigitalTwin::TAGS[ClientDigitalTwin::Tag::ROOM]) {
          auto data = response["result"].get<std::vector<std::string>>();

          for (auto &item : data) {
            item[0] = std::toupper(item[0]);
            this->nameRooms.push_back(std::move(item));
          }

          this->isReadyNameRooms = true;
        }

      } catch (...) {
        std::cerr << "[!] Client parser error" << '\n';
        isParseError = true;
      }
      this->mut.unlock();
    }
  });
}

void ClientDigitalTwin::Client::Run() { webSocket.start(); }

void ClientDigitalTwin::Client::Close() { webSocket.stop(); }

void ClientDigitalTwin::Client::Send(const std::string &method,
                                     const std::vector<std::string> &params,
                                     const std::string &tag,
                                     const size_t &id) {
  nlohmann::json request;
  request["jsonrpc"] = "2.0";
  request["method"] = method;
  request["params"] = params;
  request["tag"] = tag;
  request["id"] = id;
  webSocket.sendText(nlohmann::to_string(request));
}

bool ClientDigitalTwin::Client::IsSensorsReady() const {
  return this->isReadySensors;
};
bool ClientDigitalTwin::Client::IsError() const { return this->isParseError; };

const bool ClientDigitalTwin::Client::IsAllowUpdate() const {
  return this->isAllowUpdate;
}

void ClientDigitalTwin::Client::SetAllowUpdate(bool value) {
  this->isAllowUpdate = value;
}

bool ClientDigitalTwin::Client::IsNameRoomsReady() const {
  return this->isReadyNameRooms;
}

std::vector<std::string> ClientDigitalTwin::Client::GetNameRooms() {
  return this->nameRooms;
}