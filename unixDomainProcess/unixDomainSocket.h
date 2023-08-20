#ifndef UNIXSOCKETAPP_H
#define UNIXSOCKETAPP_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <chrono>
#include <signal.h>
#include "../include/common.h"

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/un.h>
#include <atomic>

#define SOCKET_PATH "/tmp/my_unix_socket"

class UnixSocketApp
{
public:
    UnixSocketApp();
    ~UnixSocketApp();
    void Unixinit();
    void TCPinit();
    void run();
    void setExitFlag(); // 종료 플래그 설정 함수

private:
    int m_server_fd;
    int m_new_socket;
    char m_tcpBuffer[1024];
    struct sockaddr_in m_address;
    std::atomic<bool> m_exitFlag; // 종료 플래그

    //unix
    int m_server_socket;
    struct sockaddr_un m_server_addr;
    int m_client_socket;

    void appA();
};

#endif // UNIXSOCKETAPP_H