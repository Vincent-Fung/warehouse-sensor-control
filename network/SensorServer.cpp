#include "SensorServer.hpp"
#include <syncstream>
#include <arpa/inet.h>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>

using namespace std;

namespace {
constexpr uint32_t kMaxMessageSize = 1024;

bool setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return false;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK) == 0;
}
}

SensorServer::SensorServer(int port, SensorQueue& queue)
    : m_port(port), m_queue(queue) {}

void SensorServer::run(const volatile sig_atomic_t& running) {
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd < 0) {
        cerr << "Socket-Erstellung fehlgeschlagen" << endl;
        return;
    }

    int opt = 1;
    setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(m_port);

    if (bind(listenFd, (sockaddr*)&addr, sizeof(addr)) < 0 ||
        listen(listenFd, 16) < 0 ||
        !setNonBlocking(listenFd)) {
        cerr << "bind/listen fehlgeschlagen" << endl;
        close(listenFd);
        return;
    }

    cout << "Sensor-Server lauscht auf Port " << m_port << endl;

    vector<pollfd> fds;
    fds.push_back({listenFd, POLLIN, 0});
    unordered_map<int, string> buffers;

    while (running) {
        int ready = poll(fds.data(), fds.size(), 200);   // 200 ms, damit running geprüft wird
        if (ready < 0) {
            if (errno == EINTR) continue;
            break;
        }
        if (ready == 0) continue;

        // rückwärts, damit fds.erase(i) die noch offenen Indizes nicht verschiebt
        for (size_t i = fds.size(); i-- > 0;) {
            if (fds[i].revents == 0) continue;
            int fd = fds[i].fd;

            if (fd == listenFd) {
                while (true) {
                    int clientFd = accept(listenFd, nullptr, nullptr);
                    if (clientFd < 0) break;             // EAGAIN: keine weiteren wartenden Verbindungen
                    setNonBlocking(clientFd);
                    fds.push_back({clientFd, POLLIN, 0});
                    buffers[clientFd];
                    cout << "Client verbunden (fd " << clientFd << ")" << endl;
                }
                continue;
            }

            bool closeClient = false;
            char chunk[4096];
            ssize_t n = recv(fd, chunk, sizeof(chunk), 0);
            if (n > 0) {
                buffers[fd].append(chunk, static_cast<size_t>(n));
                if (!handleData(buffers[fd])) closeClient = true;
            } else if (n == 0) {
                closeClient = true;                      // Client hat sauber geschlossen
            } else if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
                closeClient = true;                      // echter Fehler
            }

            if (closeClient) {
                close(fd);
                buffers.erase(fd);
                fds.erase(fds.begin() + static_cast<long>(i));
                cout << "Client getrennt (fd " << fd << ")" << endl;
            }
        }
    }

    for (const auto& p : fds) close(p.fd);
}

// Zerlegt den Puffer in vollständige Nachrichten. false = Verbindung trennen.
bool SensorServer::handleData(string& buffer) {
    while (buffer.size() >= 4) {
        uint32_t netLen;
        memcpy(&netLen, buffer.data(), 4);
        uint32_t len = ntohl(netLen);

        if (len == 0 || len > kMaxMessageSize) {
            cerr << "Ungueltige Laenge " << len << ", Verbindung wird getrennt" << endl;
            return false;
        }
        if (buffer.size() < 4 + len) break;              // Nachricht noch unvollständig

        processPayload(buffer.substr(4, len));
        buffer.erase(0, 4 + len);
    }
    return true;
}

void SensorServer::processPayload(const string& payload) {
    size_t sep = payload.find(';');
    if (sep == string::npos) {
        osyncstream(cerr) << "Nachricht ohne ';' verworfen: " << payload << endl;
        return;
    }
    try {
        int sensorId = stoi(payload.substr(0, sep));
        double value = stod(payload.substr(sep + 1));
        m_queue.push(SensorReading{sensorId, value});
    } catch (const exception&) {
        osyncstream(cerr) << "Ungueltige Nachricht verworfen: " << payload << endl;
    }
}