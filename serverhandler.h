#ifndef SERVERHANDLER_H
#define SERVERHANDLER_H

#include "crow.h"
#include <string>

using string = std::string;

enum class Type {
    HEAT,
    WATER,
    ELECTRICITY
};

struct ret_data {
    string text;
    Util util;
};

enum class Util {
    ON,
    OFF,
    UNKNOWN,
    CONNECTED
};

struct Data {
    Util water = Util::UNKNOWN;
    Util electricity = Util::UNKNOWN;
    Util heat = Util::UNKNOWN;
};

string status(Util util)
{
    switch (util) {
    case Util::CONNECTED:
        return "connected";
    case Util::OFF:
        return "off";
    case Util::ON:
        return "on";
    default:
        return "unknown";
    }
}


class ServerHandler
{
public:
    ServerHandler(int port = 4226);
    run();

private:
    void get(Type type);
    void post(Type type);
    ret_data judge(Type type);

private:
    crow::SimpleApp::app m_app;
    Data m_data;
    int m_port;
};

#endif // SERVERHANDLER_H
