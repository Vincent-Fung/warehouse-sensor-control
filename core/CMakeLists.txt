add_library(warehouse_core
    src/StorageLocation.cpp
    src/Article.cpp
    src/Warehouse.cpp
    src/SensorQueue.cpp
    src/FirstFitStrategy.cpp
    src/ConsolidationStrategy.cpp
)

target_include_directories(warehouse_core PUBLIC include)
target_link_libraries(warehouse_core PUBLIC Threads::Threads)
target_include_directories(warehouse_core PUBLIC include)

