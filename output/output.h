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
#include "../include/common.h"

class OutputApp
{
public:
    OutputApp();
    ~OutputApp();
    void Outputinit();
    void run();

private:
    key_t m_shm_key;
    int m_shmid;
    char *m_shmaddr;

    void appA();
};

#endif // OUTPUTAPP_H