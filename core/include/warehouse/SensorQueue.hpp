#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <vector>

#include "warehouse/Article.hpp"
#include "warehouse/SensorReading.hpp"
#include "warehouse/StorageLocation.hpp"

class SensorQueue {
private:
    std::queue<SensorReading> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    bool m_closed = false;
public:
   
    void push(const SensorReading& reading);
    std::optional<SensorReading> pop();
    void close();
};
