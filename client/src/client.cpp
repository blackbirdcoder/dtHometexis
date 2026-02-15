#include "client.hpp"
#include <nlohmann/json.hpp>
#include <iostream>

ClientDigitalTwin::Client::Client(const URL &url, const int ping) {
  std::string schema = "ws://" + url.host + ":" + std::to_string(url.port);
  webSocket.setUrl(schema);
  webSocket.setPingInterval(ping);
  isReady = false;
  isParseError = false;
}

void ClientDigitalTwin::Client::Handler(std::vector<Sensor> &sensors) {
  webSocket.setOnMessageCallback([&](const ix::WebSocketMessagePtr &msg) {
    if (msg->type == ix::WebSocketMessageType::Message) {
      mut.lock();
      try {
        nlohmann::json response = nlohmann::json::parse(msg->str);
        // TODO: Continue. Implement logic handler
        // {"id":1,"jsonrpc":"2.0","result":"Hello Client"}
        std::cout << "[**] ANSWER: " << msg->str << '\n';
        if (response["tag"] == TAGS[Tag::HOME]) {
          std::cout << "[***] IT IS HOME" << '\n';

        } else if (response["tag"] == TAGS[Tag::SENSOR]) {
          if (response["id"] != nlohmann::detail::value_t::null) {
            std::cout << "[***] IT IS SENSORS" << '\n';
            auto data = response["result"]["kitchen"]["sensors"];
            std::cout << "[***]DATA " << data << "\n";

            float posX = -4.0f;
            for (int i = 0; i < data.size(); ++i) {
              std::cout << "[" << i << "]" << data[i]["name"] << "\n";
              std::cout << "[" << i << "]" << data[i]["type"] << "\n";
              std::cout << "[" << i << "]" << data[i]["unit"] << "\n";
              std::cout << "[" << i << "]" << data[i]["value"] << "\n";
              Sensor sensor(data[i]["name"].get<std::string>(),
                            data[i]["type"].get<std::string>(),
                            data[i]["unit"].get<std::string>(),
                            data[i]["value"].get<double>(),
                            (Vector3){posX, 0.0f, 0.0f});
              sensors.push_back(sensor);
              posX += 1.2f;
            }
            isReady = true;
            std::cout << "[***]SENSOR SIZE " << sensors.size() << "\n";
          }
        }

      } catch (...) {
        std::cerr << "[!] Client parser error" << '\n';
        isParseError = true;
      }
      mut.unlock();
    }
  });
}

void ClientDigitalTwin::Client::Run() { webSocket.start(); }

void ClientDigitalTwin::Client::Close() { webSocket.stop(); }

void ClientDigitalTwin::Client::Send(const std::string &method,
                                     const std::vector<std::string> &params,
                                     const std::string &tag,
                                     const uint16_t &id) {
  nlohmann::json request;
  request["jsonrpc"] = "2.0";
  request["method"] = method;
  request["params"] = params;
  request["tag"] = tag;
  request["id"] = id;
  webSocket.sendText(nlohmann::to_string(request));
}

bool ClientDigitalTwin::Client::IsSensorsReady() const { return isReady; };
bool ClientDigitalTwin::Client::IsError() const { return isParseError; };
