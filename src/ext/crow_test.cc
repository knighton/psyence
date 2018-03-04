#include <chrono>
#include <thread>

#include "ext/crow.h"

using namespace std::chrono_literals;

namespace {

void Stop(crow::SimpleApp* app) {
    std::this_thread::sleep_for(10ms);
    app->stop();
}

}  // namespace

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([](){
        return "Hello world";
    });

    std::thread killer(Stop, &app);
    app.loglevel(crow::LogLevel::WARNING).port(18080).multithreaded().run();
    killer.join();
}
