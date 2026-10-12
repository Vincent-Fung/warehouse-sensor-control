add_executable(warehouse_tests test_warehouse.cpp)
target_link_libraries(warehouse_tests PRIVATE warehouse_core GTest::gtest_main Threads::Threads)

include(GoogleTest)
gtest_discover_tests(warehouse_tests)