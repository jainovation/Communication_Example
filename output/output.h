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

class OutputApp
{
public:
    OutputApp();
    ~OutputApp();
    void Outputinit();
    void run();
    void setExitFlag(); // 종료 플래그 설정 함수

private:
    key_t m_shm_key;
    int m_shmid;
    char *m_shmaddr;
    std::atomic<bool> m_exitFlag; // 종료 플래그

    std::mutex m_mutex;
    std::queue<std::string> m_dataQueue;

    std::string loadDataFromSharedMemory();
    void saveDataToFile();
    bool isSharedMemoryEmpty();

    void appA();
};

#endif // OUTPUTAPP_H