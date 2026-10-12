#include "warehouse/SensorQueue.hpp"


using namespace std;

void SensorQueue::push(const SensorReading& reading) {
    {
        lock_guard<mutex> lock(m_mutex);
        m_queue.push(reading);
    }
    m_condition.notify_one();
}

optional<SensorReading> SensorQueue::pop() {
    unique_lock<mutex> lock(m_mutex);
    m_condition.wait(lock, [this] {
        return !m_queue.empty() || m_closed;
    });

    if (m_queue.empty()) {
        return nullopt;
    }

    SensorReading reading = m_queue.front();
    m_queue.pop();
    return reading;
}

void SensorQueue::close() {
    {
        lock_guard<mutex> lock(m_mutex);
        m_closed = true;
    }
    m_condition.notify_all();
}