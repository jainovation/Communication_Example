#ifndef OUTPUTAPP_H
#define OUTPUTAPP_H

#include <thread>
#include <chrono>
#include <fstream>
#include <iostream>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <queue>
#include <mutex>
#include "../include/common.h"
#include <atomic>
#include <arpa/inet.h>

class OutputApp
{
public:
    OutputApp();
    ~OutputApp();
    void Outputinit();
    void run();
    void setExitFlag(); // 종료 플래그 설정 함수

    void TCPinit();

private:
    key_t m_shm_key;
    int m_shmid;
    Message_t *m_shmaddr;
    Message_t m_data;
    std::atomic<bool> m_exitFlag; // 종료 플래그

    std::mutex m_mutex;
    std::queue<std::string> m_dataQueue;

    std::string loadDataFromSharedMemory();
    void saveDataToFile();
    bool isSharedMemoryEmpty();

    // TCP 버퍼
    int m_client_fd;
    struct sockaddr_in m_server_address;

    void appA();
};

#endif // OUTPUTAPP_H