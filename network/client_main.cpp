#include "TcpClient.hpp"

int main() {
    TcpClient client("127.0.0.1", 8080);
    client.sendMessage("Sensor 1: Wert 23.5");
    return 0;
}