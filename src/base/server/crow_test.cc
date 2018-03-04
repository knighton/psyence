#include <chrono>
#include <thread>

#include "base/server/crow.h"

using psyence::base::server::crow::SimpleApp;
using namespace std::chrono_literals;

namespace {

void SleepThenStopServer(SimpleApp* app) {
    std::this_thread::sleep_for(10ms);
    app->stop();
}

}  // namespace

int main() {
    SimpleApp app;

    CROW_ROUTE(app, "/")([](){
        return "Hello world";
    });

    std::thread killer(SleepThenStopServer, &app);
    app.loglevel(crow::LogLevel::WARNING).port(18080).multithreaded().run();
    killer.join();
}
