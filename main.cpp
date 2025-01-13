#include <stdio.h>
#include <string>
#include <string.h>
#include <thread>
#include <mutex>
#include <chrono>

#include "crow.h"
#include "crow/middlewares/cors.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;
using string = std::string;

enum class Util {
    ON,
    OFF,
    UNKNOWN,
    CONNECTED
};

struct Data {
    Util water = Util::CONNECTED;
    Util electricity = Util::ON;
    Util heat = Util::ON;
    int litres = 0;
    int kilowatz = 0;
    int mc = 0;
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
    if (strcmp(status, "connected") == 0) {
        util = Util::CONNECTED;
        return;
    }
    util = Util::UNKNOWN;
}

std::mutex mtx;

void incrementUsage()
{
    std::unique_lock<std::mutex> lock(mtx);
    while(true) {
        if (data.water == Util::CONNECTED) {
            data.litres += 1;
        }
        if (data.heat == Util::ON) {
            data.mc += 1;
        }
        if (data.electricity == Util::ON) {
            data.kilowatz += 1;
        }
        printf("%d l\n%d mc\n%d kw\n", data.litres, data.mc, data.kilowatz);
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(3));
        lock.lock();
    }
}

int main()
{
    crow::App<crow::CORSHandler> app;

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
        response_body["water"] = data.litres;

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
            std::lock_guard<std::mutex> lock(mtx);
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
        response_body["heat"] = data.mc;

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
            std::lock_guard<std::mutex> lock(mtx);
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
        response_body["electricity"] = data.kilowatz;

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
            std::lock_guard<std::mutex> lock(mtx);
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

    auto startServer = [&](){
        app.port(4226).multithreaded().run();
    };
    std::thread asyncThread(incrementUsage);

    // Start the server in a separate thread asynchronously
    std::thread serverThread(startServer);

    // Wait for the server to finish (i.e., the server is running indefinitely)
    serverThread.join();

    // Wait for the async background thread to finish
    asyncThread.join();

    return 0;
}
