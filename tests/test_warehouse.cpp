#include <gtest/gtest.h>
#include "warehouse/Article.hpp"
#include "warehouse/StorageLocation.hpp"
#include "warehouse/Warehouse.hpp"
#include "warehouse/FirstFitStrategy.hpp"
#include "warehouse/ConsolidationStrategy.hpp"
#include "warehouse/SensorQueue.hpp"
#include <thread>
#include <chrono>

using namespace std;

// --- StorageLocation ---

TEST(StorageLocationTest, CanFitOnEmptyLocation) {
    StorageLocation loc(1, 10.0, 0.0, nullopt);
    Article article(1, "Schraube", 1.0);
    EXPECT_TRUE(loc.canFit(article, 5.0));
}

TEST(StorageLocationTest, CannotFitDifferentArticleOnOccupiedLocation) {
    StorageLocation loc(1, 10.0, 0.0, nullopt);
    Article article1(1, "Schraube", 1.0);
    Article article2(2, "Mutter", 1.0);

    loc.assign(article1, 3.0);

    EXPECT_FALSE(loc.canFit(article2, 2.0));
}

TEST(StorageLocationTest, AssignThrowsWhenOverCapacity) {
    StorageLocation loc(1, 10.0, 0.0, nullopt);
    Article article(1, "Schraube", 1.0);

    EXPECT_THROW(loc.assign(article, 100.0), std::runtime_error);
}

TEST(StorageLocationTest, ReleaseResetsArticleWhenEmpty) {
    StorageLocation loc(1, 10.0, 0.0, nullopt);
    Article article(1, "Schraube", 1.0);

    loc.assign(article, 5.0);
    loc.release(5.0);

    EXPECT_FALSE(loc.getStoredArticleId().has_value());
}

// --- FirstFitStrategy ---

TEST(FirstFitStrategyTest, PlacesOnFirstAvailableLocation) {
    vector<StorageLocation> locations = {
        StorageLocation(1, 10.0, 0.0, nullopt),
        StorageLocation(2, 10.0, 0.0, nullopt),
    };
    Article article(1, "Schraube", 1.0);
    FirstFitStrategy strategy;

    bool result = strategy.place(locations, article, 5.0);

    EXPECT_TRUE(result);
    EXPECT_TRUE(locations[0].getStoredArticleId().has_value());
}

TEST(FirstFitStrategyTest, ReturnsFalseWhenNothingFits) {
    vector<StorageLocation> locations = {
        StorageLocation(1, 10.0, 0.0, nullopt),
    };
    Article article(1, "Schraube", 1.0);
    FirstFitStrategy strategy;

    bool result = strategy.place(locations, article, 100.0);

    EXPECT_FALSE(result);
}

// --- ConsolidationStrategy ---

TEST(ConsolidationStrategyTest, PrefersLocationWithSameArticle) {
    vector<StorageLocation> locations = {
        StorageLocation(1, 10.0, 0.0, nullopt),
        StorageLocation(2, 10.0, 0.0, nullopt),
    };
    Article article(1, "Schraube", 1.0);
    ConsolidationStrategy strategy;

    // Erste Einlagerung: landet auf Platz 1 (First-Fit-Fallback, da noch nichts belegt)
    strategy.place(locations, article, 4.0);
    // Zweite Einlagerung desselben Artikels: sollte wieder auf Platz 1 landen (Konsolidierung)
    strategy.place(locations, article, 3.0);

    EXPECT_TRUE(locations[0].getStoredArticleId().has_value());
    EXPECT_FALSE(locations[1].getStoredArticleId().has_value());
}

// --- Warehouse (Integration) ---

TEST(WarehouseTest, StoreArticleSucceedsWithValidData) {
    vector<Article> articles = { Article(1, "Schraube", 1.0) };
    vector<StorageLocation> locations = { StorageLocation(1, 10.0, 0.0, nullopt) };

    Warehouse warehouse(locations, articles, make_unique<FirstFitStrategy>());

    EXPECT_TRUE(warehouse.storeArticle(1, 5.0));
}

TEST(WarehouseTest, StoreArticleFailsForUnknownArticleId) {
    vector<Article> articles = { Article(1, "Schraube", 1.0) };
    vector<StorageLocation> locations = { StorageLocation(1, 10.0, 0.0, nullopt) };

    Warehouse warehouse(locations, articles, make_unique<FirstFitStrategy>());

    EXPECT_FALSE(warehouse.storeArticle(999, 5.0));
}

// --- SensorQueue ---

TEST(SensorQueueTest, PushThenPopReturnsSameValue) {
    SensorQueue queue;
    queue.push(SensorReading{1, 42.0});

    auto result = queue.pop();

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->sensorId, 1);
    EXPECT_EQ(result->value, 42.0);
}

TEST(SensorQueueTest, PopBlocksUntilPushHappens) {
    SensorQueue queue;
    bool popped = false;

    thread consumer([&queue, &popped]() {
        auto result = queue.pop();
        popped = result.has_value();
    });

    // Kurz warten, um sicherzugehen, dass der Consumer wirklich schon in pop() wartet
    this_thread::sleep_for(chrono::milliseconds(100));
    EXPECT_FALSE(popped);  // sollte noch NICHT fertig sein, da noch nichts gepusht wurde

    queue.push(SensorReading{2, 10.0});
    consumer.join();

    EXPECT_TRUE(popped);  // jetzt sollte pop() zurückgekehrt sein
}

TEST(SensorQueueTest, PopReturnsNulloptAfterCloseOnEmptyQueue) {
    SensorQueue queue;

    thread consumer([&queue]() {
        auto result = queue.pop();
        EXPECT_FALSE(result.has_value());
    });

    this_thread::sleep_for(chrono::milliseconds(100));
    queue.close();
    consumer.join();
}