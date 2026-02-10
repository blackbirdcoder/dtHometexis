#pragma once
#include <ixwebsocket/IXWebSocket.h>
#include <mutex>
#include "settings.hpp"
#include "sensor.hpp"
#include <vector>

namespace ClientDigitalTwin {
class Client {
public:
  
  Client(const URL &url, const int ping);
  void Handler(std::vector<Sensor> &sensors);
  void Run();
  void Close();
  void Send(const std::string &method, const std::vector<std::string> &params,
            const std::string &tag, const id_measure &id);
  bool GetSensorReady() const;
  bool ErrorOccurred() const;

private:
  ix::WebSocket webSocket;
  std::mutex mut;
  bool sensorsReady;
  bool isError;
};

} // namespace ClientDigitalTwin