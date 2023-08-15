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
#include "../include/common.h"

class SharedMemoryApp
{
public:
    SharedMemoryApp();
    ~SharedMemoryApp();
    void SharedMemoryinit();
    void UDPinit();
    void run();

private:
    key_t m_shm_key;
    int m_shmid;
    char *m_shmaddr;

    char m_udpBuffer[1024];
    int m_udp_socket;
    struct sockaddr_in m_server_address;
    struct sockaddr_in m_client_address;
    socklen_t m_client_address_len;

    void appA();
};

#endif // SHAREDMEMORYAPP_H