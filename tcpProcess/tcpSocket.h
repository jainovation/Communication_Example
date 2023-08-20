#ifndef TCPSOCKETAPP_H
#define TCPSOCKETAPP_H

#include <mqueue.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include "../include/common.h"
#include <atomic>

#define MAX_MSG_SIZE 1024

class TcpSocketApp
{
public:
    TcpSocketApp();
    ~TcpSocketApp();
    void run();
    int getMQdata();
    int getTCPClient();
    void MQinit();
    void TCPinit();
    void setExitFlag(); // 종료 플래그 설정 함수

private:
    mqd_t m_mq;
    int m_client_fd;
    std::atomic<bool> m_exitFlag; // 종료 플래그

    // MQ 버퍼
    char m_mqBuffer[MAX_MSG_SIZE];

    // TCP 버퍼
    struct sockaddr_in m_server_address;


    void appA();
};

#endif // TCPSOCKETAPP_H