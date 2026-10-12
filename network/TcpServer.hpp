#pragma once
#include <string>

class TcpServer {
public:
    explicit TcpServer(int port);
    void run();

private:
    int m_port;
};