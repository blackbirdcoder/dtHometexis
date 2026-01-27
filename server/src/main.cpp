#include <iostream>
#include "../external/httplib/httplib.h"
#include "../external/inih/INIReader.h"
#include <string>
#include "settings.h"

const std::string ServerDigitalTwin::CONFIG_FILE = "settings.ini";

int main() {
  INIReader reader(ServerDigitalTwin::CONFIG_FILE);

  if (reader.ParseError() < 0) {
    std::cerr << "[!] Error: Can't load "
              << "'" << ServerDigitalTwin::CONFIG_FILE << "'\n";
    return 1;
  }

  ServerDigitalTwin::URL url = {
      reader.Get("server", "host", "localhost"),
      reader.GetInteger("server", "port", 5000),
      reader.Get("server", "data_path", "/data"),
  };

  httplib::Server svr;
  svr.Get(url.dataPath, [](const httplib::Request &, httplib::Response &res) {
    res.set_content("Hello App!", "text/plain");
  });

  std::cout << "[->] Magical Server for a Digital Twin of Smart Home\n"
            << "[->] Run: "
            << "http://" << url.host << ":" << url.port << url.dataPath << '\n';
  svr.listen(url.host, url.port);

  return 0;
}