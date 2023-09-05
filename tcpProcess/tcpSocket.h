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
    void saveDataToFile();
    bool receiveMessage(Message_t& message);
    bool sendMessage(const Message_t &message);

private:
    std::atomic<bool> m_exitFlag; // 종료 플래그

    // MQ 버퍼
    mqd_t m_mq;
    char m_mqBuffer[MAX_MSG_SIZE];

    // TCP 버퍼
    int m_client_fd;
    struct sockaddr_in m_server_address;

    Message_t m_receivedMessage;  // 수신한 메시지를 저장할 구조체

    void appA();
};

#endif // TCPSOCKETAPP_H