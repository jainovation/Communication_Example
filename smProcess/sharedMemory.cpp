#include "sharedMemory.h"

SharedMemoryApp app;

static void exitHandler(int signalNum)
{
    if (signalNum == SIGINT || signalNum == SIGTERM || signalNum == SIGKILL)
    {
        // close(m_client_socket);
        // close(m_server_socket);
    }
}

SharedMemoryApp::SharedMemoryApp()
{
    m_udp_socket = -1;
    m_shm_key = 0;
    m_shmid = -1;
    m_shmaddr = nullptr;
}

SharedMemoryApp::~SharedMemoryApp()
{
    if (m_udp_socket != -1)
    {
        close(m_udp_socket);
    }
    if (m_shmaddr != nullptr)
    {
        shmdt(m_shmaddr);
    }
}

void SharedMemoryApp::SharedMemoryinit()
{
    m_shm_key = ftok("shared_memory_key", 65); // 키 생성

    // 공유 메모리 생성 및 연결
    m_shmid = shmget(m_shm_key, 1024, 0666 | IPC_CREAT);
    m_shmaddr = (char *)shmat(m_shmid, (void *)0, 0);

    std::cerr << "shared memory init" << std::endl;
}

void SharedMemoryApp::UDPinit()
{
    // 소켓 생성
    if ((m_udp_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket");
        // return 1;
    }

    m_server_address.sin_family = AF_INET;
    m_server_address.sin_addr.s_addr = INADDR_ANY;
    m_server_address.sin_port = htons(23456); // 포트 번호

    // 소켓 바인딩
    if (bind(m_udp_socket, (struct sockaddr *)&m_server_address, sizeof(m_server_address)) == -1)
    {
        perror("bind");
        // return 1;
    }

    m_client_address_len = sizeof(m_client_address);

    std::cerr << "udp Socket Connected" << std::endl;
}

void SharedMemoryApp::appA()
{
    while (true)
    {
        memset(m_udpBuffer, 0x00, sizeof(m_udpBuffer));

        // 클라이언트로부터 데이터 수신
        ssize_t bytes_received = recvfrom(m_udp_socket, m_udpBuffer, sizeof(m_udpBuffer), 0, (struct sockaddr *)&m_client_address, &m_client_address_len);
        if (bytes_received == -1)
        {
            perror("recvfrom");
            sleep(1);
            // break;
        }
        else
        {
            // 데이터 쓰기
            std::string message = m_udpBuffer;
            // strcpy(m_shmaddr, message.c_str());

            // std::cout << "Received message from unixTask: " << m_udpBuffer << std::endl;

            // 데이터 쓰기
            int nextIndex = *reinterpret_cast<int *>(m_shmaddr);
            if (nextIndex + message.size() + 1 <= 1024)
            {
                std::strcpy(m_shmaddr + sizeof(int) + nextIndex, message.c_str());
                *reinterpret_cast<int *>(m_shmaddr) = nextIndex + message.size() + 1;
                std::cout << "Data written to shared memory: " << message << std::endl;
            }
        }
    }
}

void SharedMemoryApp::run()
{
    std::cout << "App SharedMemory is running..." << std::endl;

    signal(SIGINT, exitHandler);
    signal(SIGTERM, exitHandler);
    signal(SIGKILL, exitHandler);

    UDPinit();
    SharedMemoryinit();

    appA();
}

int main()
{
    app.run();

    return 0;
}