#include "serverhandler.h"

ServerHandler::ServerHandler(int port)
    : m_port(port)
{

}

ret_data ServerHandler::judge(Type type)
{

}

string ServerHandler::status(Util util)
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
