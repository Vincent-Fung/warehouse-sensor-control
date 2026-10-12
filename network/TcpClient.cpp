#include "TcpClient.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace std;

TcpClient::TcpClient(const string& host, int port) : m_host(host), m_port(port) {}

void TcpClient::sendMessage(const string& message) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cerr << "Socket-Erstellung fehlgeschlagen" << endl;
        return;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(m_port);

    // IP-Adresse als String ("127.0.0.1") in binäre Netzwerk-Darstellung umwandeln
    if (inet_pton(AF_INET, m_host.c_str(), &serverAddress.sin_addr) <= 0) {
        cerr << "Ungueltige Adresse: " << m_host << endl;
        close(sock);
        return;
    }

    if (connect(sock, (sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        cerr << "Verbindung fehlgeschlagen" << endl;
        close(sock);
        return;
    }

    send(sock, message.c_str(), message.size(), 0);
    cout << "Gesendet: " << message << endl;

    close(sock);
}