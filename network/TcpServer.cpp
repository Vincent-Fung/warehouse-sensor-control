#include "TcpServer.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

TcpServer::TcpServer(int port) : m_port(port) {}

void TcpServer::run() {
    // 1. Socket erstellen: IPv4 (AF_INET), TCP (SOCK_STREAM)
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0) {
        cerr << "Socket-Erstellung fehlgeschlagen" << endl;
        return;
    }

    // Erlaubt sofortiges Wiederverwenden des Ports nach Programmende
    // (sonst "Address already in use" bei schnellem Neustart)
    int opt = 1;
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2. Adresse konfigurieren: auf allen Netzwerk-Interfaces lauschen
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(m_port);  // htons: Host-Byte-Order -> Netzwerk-Byte-Order

    // 3. Socket an die Adresse binden
    if (bind(serverFd, (sockaddr*)&address, sizeof(address)) < 0) {
        cerr << "Bind fehlgeschlagen" << endl;
        close(serverFd);
        return;
    }

    // 4. In den "Listen"-Modus versetzen, max. 5 wartende Verbindungen in der Warteschlange
    if (listen(serverFd, 5) < 0) {
        cerr << "Listen fehlgeschlagen" << endl;
        close(serverFd);
        return;
    }

    cout << "Server lauscht auf Port " << m_port << "..." << endl;

    // 5. Auf eine eingehende Verbindung warten (blockiert, bis ein Client sich verbindet)
    sockaddr_in clientAddress{};
    socklen_t clientLen = sizeof(clientAddress);
    int clientFd = accept(serverFd, (sockaddr*)&clientAddress, &clientLen);
    if (clientFd < 0) {
        cerr << "Accept fehlgeschlagen" << endl;
        close(serverFd);
        return;
    }

    cout << "Client verbunden." << endl;

    // 6. Daten empfangen (blockiert, bis was ankommt)
    char buffer[1024] = {0};
    ssize_t bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead > 0) {
        cout << "Empfangen: " << buffer << endl;
    }

    // 7. Aufräumen
    close(clientFd);
    close(serverFd);
}