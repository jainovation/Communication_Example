#ifndef UDPSOCKETAPP_H
#define UDPSOCKETAPP_H

#include <thread>
#include <chrono>
#include <signal.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../include/common.h"
#include <atomic>

class UdpSocketApp
{
public:
    UdpSocketApp();
    ~UdpSocketApp();
    void Unixinit();
    void UDPinit();
    void run();
    void setExitFlag(); // 종료 플래그 설정 함수

private:
    int m_client_socket;
    struct sockaddr_un m_server_addr;
    char m_unixBuffer[1024];
    std::atomic<bool> m_exitFlag; // 종료 플래그

    //udp
    int m_udp_socket;
    struct sockaddr_in m_server_address;

    void appA();
};

#endif // UDPSOCKETAPP_H