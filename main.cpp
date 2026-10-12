#include <iostream>
#include "warehouse/Article.hpp"
#include "warehouse/StorageLocation.hpp"
#include "warehouse/Warehouse.hpp"
#include <chrono>
#include <thread>
#include "warehouse/SensorQueue.hpp"
#include "warehouse/ConsolidationStrategy.hpp"
#include "warehouse/Statistics.hpp"

using namespace std;

void producerFunction(SensorQueue& queue, int sensorId, int count);
void consumerFunction(SensorQueue& queue);

mutex coutMutex;

int main()
{
    vector<Article> articles = {
        Article(1, "Schraube M5", 1.0),
        Article(2, "Mutter M5", 1.0),
        Article(3, "Unterlegscheibe M5", 1.0),
    };

    vector<StorageLocation> locations = {
        StorageLocation(1, 10.0, 0.0, nullopt),
        StorageLocation(2, 10.0, 0.0, nullopt),
        StorageLocation(3, 10.0, 0.0, nullopt),
    };

    Warehouse warehouse(locations, articles, make_unique<ConsolidationStrategy>());
    
    bool test1 = warehouse.storeArticle(1, 4);
    cout << "Test 1 (sollte true sein): " << test1 << endl;

    bool test2 = warehouse.storeArticle(1, 3);
    cout << "Test 2 (sollte true sein, Konsolidierung): " << test2 << endl;

    bool test3 = warehouse.storeArticle(1, 100);
    cout << "Test 3 (sollte false sein): " << test3 << endl;

    bool test4 = warehouse.storeArticle(3, 9);
    cout << "Test 4 (sollte true sein): " << test4 << endl;

    bool test5 = warehouse.storeArticle(3, 1);
    cout << "Test 5 (sollte true sein): " << test5 << endl;

    bool test6 = warehouse.storeArticle(3, 1);
    cout << "Test 6 (sollte true sein): " << test6 << endl;






   
SensorQueue sensorQueue;

vector<thread> producers;
for (int sensorId = 1; sensorId <= 3; sensorId++) {
    producers.emplace_back(producerFunction, ref(sensorQueue), sensorId, 5);
}

vector<thread> consumers;
for (int i = 0; i < 2; i++) {
    consumers.emplace_back(consumerFunction, ref(sensorQueue));
}

for (auto& producer : producers) {
    producer.join();
}

sensorQueue.close();

for (auto& consumer : consumers) {
    consumer.join();
}



vector<double> readings = {20.0, 23.5, 27.0, 30.5};
double avg = computeAverage(readings);
cout << "Durchschnitt: " << avg << endl;

    return 0;
}




void producerFunction(SensorQueue& queue, int sensorId, int count) {
    for (int i = 0; i < count; i++) {
        SensorReading reading{sensorId, 20.0 + i * 3.5};
        queue.push(reading);
        {
            lock_guard<mutex> lock(coutMutex);
            cout << "Produced: sensor " << reading.sensorId << " = " << reading.value << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

void consumerFunction(SensorQueue& queue) {
    while (true) {
        optional<SensorReading> reading = queue.pop();
        if (!reading.has_value()) {
            lock_guard<mutex> lock(coutMutex);
            cout << "Consumer: Queue closed." << endl;
            break;
        }
        {
            lock_guard<mutex> lock(coutMutex);
            cout << "Consumed: sensor " << reading->sensorId << " = " << reading->value << endl;
        }

    }
}