#pragma once
#include <string>

class TcpClient {
public:
    TcpClient(const std::string& host, int port);
    void sendMessage(const std::string& message);

private:
    std::string m_host;
    int m_port;
};