#include "settings.hpp"

const std::string ClientDigitalTwin::CONFIG_FILE{"settings.ini"};
const int ClientDigitalTwin::PING_INTERVAL{45};
const std::array<std::string, 4> ClientDigitalTwin::TAGS{
    "home",
    "sensor",
    "update",
    "set",
};
const int ClientDigitalTwin::SIZE{16};
