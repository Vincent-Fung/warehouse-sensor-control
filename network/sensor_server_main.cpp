#include <csignal>
#include <iostream>
#include <thread>
#include "SensorServer.hpp"
#include <syncstream>

using namespace std;

static volatile sig_atomic_t g_running = 1;
static void onSignal(int) { g_running = 0; }

int main() {
    signal(SIGINT, onSignal);
    signal(SIGTERM, onSignal);

    SensorQueue queue;

    thread consumer([&queue] {
        while (auto reading = queue.pop()) {
        osyncstream(cout) << "Consumed: sensor " << reading->sensorId
                        << " = " << reading->value << endl;
        }
        cout << "Consumer beendet" << endl;
    });

    SensorServer server(9000, queue);
    server.run(g_running);

    queue.close();
    consumer.join();
    return 0;
}