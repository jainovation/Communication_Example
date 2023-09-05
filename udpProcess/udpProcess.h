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
#include <fstream>

class UdpSocketApp
{
public:
    UdpSocketApp();
    ~UdpSocketApp();
    void Unixinit();
    void UDPinit();
    void run();
    void setExitFlag(); // 종료 플래그 설정 함수
    void saveDataToFile();
    bool receiveMessage(Message_t& message);
    bool sendMessage(const Message_t &message);
    bool retryMessageFromUDP();

private:
    int m_client_socket;
    struct sockaddr_un m_server_addr;
    char m_unixBuffer[PACKET_SIZE];
    std::atomic<bool> m_exitFlag; // 종료 플래그

    //udp
    int m_udp_socket;
    struct sockaddr_in m_server_address;
    socklen_t m_server_addr_len;
    char m_ackBuffer[1];

    Message_t m_receivedMessage;  // 수신한 메시지를 저장할 구조체

    void appA();
};

#endif // UDPSOCKETAPP_H