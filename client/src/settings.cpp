#include "settings.hpp"

const std::string ClientDigitalTwin::CONFIG_FILE{"settings.ini"};
const int ClientDigitalTwin::PING_INTERVAL{45};
const std::array<std::string, 5> ClientDigitalTwin::TAGS{
    "home",
    "sensor",
    "update",
    "set",
    "room",
};
const int ClientDigitalTwin::SIZE{16};
const std::string ClientDigitalTwin::DB_NAME = "hometexis.db";
