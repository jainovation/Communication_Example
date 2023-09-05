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
#include <fstream>

class UnixSocketApp
{
public:
    UnixSocketApp();
    ~UnixSocketApp();
    void Unixinit();
    void TCPinit();
    void run();
    void setExitFlag(); // 종료 플래그 설정 함수
    void saveDataToFile();
    bool receiveMessage(Message_t& message);
    bool sendMessage(const Message_t &message);

private:
    //tcp
    int m_server_fd;
    int m_new_socket;
    char m_tcpBuffer[PACKET_SIZE];
    struct sockaddr_in m_address;

    std::atomic<bool> m_exitFlag; // 종료 플래그

    //unix
    int m_server_socket;
    struct sockaddr_un m_server_addr;
    int m_client_socket;

    Message_t m_receivedMessage;  // 수신한 메시지를 저장할 구조체

    void appA();
};

#endif // UNIXSOCKETAPP_H