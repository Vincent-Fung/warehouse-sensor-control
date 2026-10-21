#include <arpa/inet.h>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

using namespace std;

static bool sendAll(int fd, const char* data, size_t len) {
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = send(fd, data + sent, len - sent, MSG_NOSIGNAL);
        if (n <= 0) return false;
        sent += static_cast<size_t>(n);
    }
    return true;
}

static bool sendMessage(int fd, const string& payload) {
    uint32_t netLen = htonl(static_cast<uint32_t>(payload.size()));
    string frame(reinterpret_cast<const char*>(&netLen), sizeof(netLen));
    frame += payload;
    return sendAll(fd, frame.data(), frame.size());
}

int main(int argc, char* argv[]) {
    int sensorId = argc > 1 ? atoi(argv[1]) : 1;
    int count    = argc > 2 ? atoi(argv[2]) : 5;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9000);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        cerr << "Verbindung fehlgeschlagen" << endl;
        return 1;
    }

    for (int i = 0; i < count; i++) {
        string payload = to_string(sensorId) + ";" + to_string(20.0 + i * 1.5);
        if (!sendMessage(sock, payload)) {
            cerr << "Senden fehlgeschlagen" << endl;
            break;
        }
        cout << "Gesendet: " << payload << endl;
        this_thread::sleep_for(chrono::milliseconds(200));
    }

    close(sock);
    return 0;
}