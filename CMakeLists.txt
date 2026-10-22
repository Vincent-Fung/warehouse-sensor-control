cmake_minimum_required(VERSION 3.20)
project(WarehouseSensorControl CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Threads REQUIRED)

add_subdirectory(core)
add_executable(warehouse_test main.cpp)
target_link_libraries(warehouse_test PRIVATE warehouse_core)
add_executable(tcp_server network/TcpServer.cpp network/server_main.cpp)
target_link_libraries(tcp_server PRIVATE Threads::Threads)

add_executable(tcp_client network/TcpClient.cpp network/client_main.cpp)
target_link_libraries(tcp_client PRIVATE Threads::Threads)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_subdirectory(tests)

execute_process(
    COMMAND python3 -m pybind11 --cmakedir
    OUTPUT_VARIABLE pybind11_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
find_package(pybind11 REQUIRED)
pybind11_add_module(warehouse_py python/bindings.cpp)
target_link_libraries(warehouse_py PRIVATE warehouse_core)

add_executable(sensor_server network/SensorServer.cpp network/sensor_server_main.cpp)
target_link_libraries(sensor_server PRIVATE warehouse_core Threads::Threads)

add_executable(sensor_client network/sensor_client_main.cpp)
target_link_libraries(sensor_client PRIVATE Threads::Threads)

find_package(OpenSSL REQUIRED)

add_executable(tls_server network/tls_server_main.cpp)
target_link_libraries(tls_server PRIVATE OpenSSL::SSL OpenSSL::Crypto)

add_executable(tls_client network/tls_client_main.cpp)
target_link_libraries(tls_client PRIVATE OpenSSL::SSL OpenSSL::Crypto)