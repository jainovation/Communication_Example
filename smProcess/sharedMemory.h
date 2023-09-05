#ifndef SHAREDMEMORYAPP_H
#define SHAREDMEMORYAPP_H

#include <thread>
#include <chrono>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <queue>
#include <mutex>
#include "../include/common.h"
#include <atomic>
#include <fstream>
#include <unistd.h>
#include <arpa/inet.h>

class SharedMemoryApp
{
public:
    SharedMemoryApp();
    ~SharedMemoryApp();
    void SharedMemoryinit();
    void UDPinit();
    void run();
    void setExitFlag(); // 종료 플래그 설정 함수
    void saveDataToFile();
    bool receiveMessage(Message_t& message);
    // bool sendMessage(const Message_t &message);
    bool checkRetryMessage(Message_t& message);

    void TCPinit();

private:
    //shared
    key_t m_shm_key;
    int m_shmid;
    // char *m_shmaddr;
    Message_t *m_shmaddr;
    std::mutex m_mutex;
    std::queue<std::string> m_dataQueue;
    std::atomic<bool> m_exitFlag; // 종료 플래그

    //udp
    char m_udpBuffer[PACKET_SIZE];
    int m_udp_socket;
    struct sockaddr_in m_server_address;
    struct sockaddr_in m_client_address;
    socklen_t m_client_address_len;

    Message_t m_receivedMessage;  // 수신한 메시지를 저장할 구조체

    //tcp
    //tcp
    int m_server_fd;
    int m_new_socket;
    struct sockaddr_in m_address;

    void appA();
    void appB();
    void sendDataToSharedMemory(const std::string &data);
    bool isSharedMemoryEmpty();
};

#endif // SHAREDMEMORYAPP_H