#include "crow.h"
#include "nlohmann/json.hpp"
#include <string>
#include <string.h>

using json = nlohmann::json;
using string = std::string;

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
} data;

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

void change(Util &util, const char* status)
{
    if (strcmp(status, "on") == 0) {
        util = Util::ON;
        return;
    }
    if (strcmp(status, "off") == 0) {
        util = Util::OFF;
        return;
    }
    if (strcmp(status, "connceted") == 0) {
        util = Util::CONNECTED;
        return;
    }
    util = Util::UNKNOWN;
}

int main()
{
    crow::SimpleApp app;

    /****************
     *****SYSTEM*****
     ****************/

    CROW_ROUTE(app, "/api/system") ([](){
        json response_body;
        response_body["water"] = status(data.water).c_str();
        response_body["heat"] = status(data.heat).c_str();
        response_body["electricity"] = status(data.electricity).c_str();

        crow::response res(response_body.dump());
        res.set_header("Content-Type", "application/json");
        res.set_header("Server", "Crow/1.0");
        res.code = 200;
        return res;
    });

    /***************
     *****WATER*****
     ***************/
    CROW_ROUTE(app, "/api/water") ([](){
        json response_body;
        response_body["water"] = status(data.water);

        crow::response res(response_body.dump());

        res.set_header("Content-Type", "application/json");
        res.set_header("Server", "Crow/1.0");
        res.code = 200;
        return res;
    });

    CROW_ROUTE(app, "/api/water").methods("POST"_method) ([](const crow::request& req){
        json request_body = json::parse(req.body);
        string req_text = request_body.contains("value") ? request_body["value"].get<string>() : "";

        if (!req_text.empty()) {
            change(data.water, req_text.c_str());
            crow::response res;
            res.set_header("Server", "Crow/1.0");
            res.code = 200;
            return res;
        }

        crow::response res;
        res.set_header("server", "Crow/1.0");
        res.code = 400;
        return res;
    });

    /****************
     ******HEAT******
     ****************/

    CROW_ROUTE(app, "/api/heat") ([](){
        json response_body;
        response_body["heat"] = status(data.heat);

        crow::response res(response_body.dump());

        res.set_header("Content-Type", "application/json");
        res.set_header("Server", "Crow/1.0");
        res.code = 200;
        return res;
    });

    CROW_ROUTE(app, "/api/heat").methods("POST"_method) ([](const crow::request& req){
        json request_body = json::parse(req.body);
        string req_text = request_body.contains("value") ? request_body["value"].get<string>() : "";

        if (!req_text.empty()) {
            change(data.heat, req_text.c_str());
            crow::response res;
            res.set_header("Server", "Crow/1.0");
            res.code = 200;
            return res;
        }

        crow::response res;
        res.set_header("server", "Crow/1.0");
        res.code = 400;
        return res;
    });

    /****************
     ******ELEC******
     ****************/

    CROW_ROUTE(app, "/api/electricity") ([](){
        json response_body;
        response_body["electricity"] = status(data.electricity);

        crow::response res(response_body.dump());

        res.set_header("Content-Type", "application/json");
        res.set_header("Server", "Crow/1.0");

        res.code = 200;

        return res;
    });

    CROW_ROUTE(app, "/api/electricity").methods("POST"_method) ([](const crow::request& req){
        json request_body = json::parse(req.body);
        string req_text = request_body.contains("value") ? request_body["value"].get<string>() : "";

        if (!req_text.empty()) {
            change(data.electricity, req_text.c_str());
            crow::response res;
            res.set_header("Server", "Crow/1.0");
            res.code = 200;
            return res;
        }

        crow::response res;
        res.set_header("server", "Crow/1.0");
        res.code = 400;
        return res;
    });

    app.port(4226).multithreaded().run();
    return 0;
}
