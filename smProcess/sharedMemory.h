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

class SharedMemoryApp
{
public:
    SharedMemoryApp();
    ~SharedMemoryApp();
    void SharedMemoryinit();
    void UDPinit();
    void run();
    void setExitFlag(); // 종료 플래그 설정 함수

private:
    key_t m_shm_key;
    int m_shmid;
    char *m_shmaddr;
    std::mutex m_mutex;
    std::queue<std::string> m_dataQueue;
    std::atomic<bool> m_exitFlag; // 종료 플래그

    char m_udpBuffer[1024];
    int m_udp_socket;
    struct sockaddr_in m_server_address;
    struct sockaddr_in m_client_address;
    socklen_t m_client_address_len;

    void appA();
    void appB();
    void sendDataToSharedMemory(const std::string &data);
    bool isSharedMemoryEmpty();
};

#endif // SHAREDMEMORYAPP_H