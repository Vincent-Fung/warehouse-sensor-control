#include "TcpServer.hpp"

int main() {
    TcpServer server(8080);
    server.run();
    return 0;
}