#pragma once
#include <csignal>
#include <string>
#include "warehouse/SensorQueue.hpp"

class SensorServer {
public:
    SensorServer(int port, SensorQueue& queue);
    void run(const volatile std::sig_atomic_t& running);

private:
    bool handleData(std::string& buffer);
    void processPayload(const std::string& payload);

    int m_port;
    SensorQueue& m_queue;
};