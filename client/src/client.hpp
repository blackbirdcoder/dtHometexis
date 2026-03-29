#pragma once
#include <ixwebsocket/IXWebSocket.h>
#include <mutex>
#include "settings.hpp"
#include "sensors/sensor.hpp"
#include <vector>

namespace ClientDigitalTwin {
class Client {
public:
  Client(const URL &url, const int ping);
  void Handler(std::vector<std::unique_ptr<ClientDigitalTwin::Sensor>> &sensors,
               Mode mode);
  void Run();
  void Close();
  void Send(const std::string &method, const std::vector<std::string> &params,
            const std::string &tag, const size_t &id);
  bool IsSensorsReady() const;
  bool IsError() const;
  const bool IsAllowUpdate() const;
  void SetAllowUpdate(bool value);
  bool IsNameRoomsReady() const;
  std::vector<std::string> GetNameRooms();

private:
  ix::WebSocket webSocket;
  std::mutex mut;
  bool isReadySensors;
  bool isParseError;
  bool isAllowUpdate;
  bool isReadyNameRooms;
  std::vector<std::string> nameRooms;
};

} // namespace ClientDigitalTwin