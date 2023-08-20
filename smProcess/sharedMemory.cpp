#include "sharedMemory.h"

SharedMemoryApp app;

void SharedMemoryApp::setExitFlag() {
    m_exitFlag.store(true);
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
    std::cerr << "shared memory id:" << m_shmid << std::endl;
    m_shmaddr = (char *)shmat(m_shmid, (void *)0, 0);

    memset(m_shmaddr, 0x00, 1024); // 공유 메모리 초기화

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

void SharedMemoryApp::sendDataToSharedMemory(const std::string &data)
{
    // std::lock_guard<std::mutex> lock(m_mutex);
    // 데이터 길이가 공유 메모리 크기보다 작을 경우에만 쓰기
    if (data.size() < 1024) {
        // memset(m_shmaddr, 0x00, 1024); // 공유 메모리 초기화
        strcpy(m_shmaddr, data.c_str()); // 데이터 쓰기
    }
}

bool SharedMemoryApp::isSharedMemoryEmpty() {
    return m_shmaddr[0] == '\0';
}

void SharedMemoryApp::appA()
{
    while (!m_exitFlag.load())
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

            std::cout << "Received message from unixTask: " << m_udpBuffer << std::endl;

            // 데이터 쓰기
        #if 0
            int nextIndex = *reinterpret_cast<int *>(m_shmaddr);
            if (nextIndex + message.size() + 1 <= 1024)
            {
                std::strcpy(m_shmaddr + sizeof(int) + nextIndex, message.c_str());
                *reinterpret_cast<int *>(m_shmaddr) = nextIndex + message.size() + 1;
                std::cout << "Data written to shared memory: " << message << std::endl;
            }
        #else
            std::lock_guard<std::mutex> lock(m_mutex);
            m_dataQueue.push(message);
        #endif
        }


    }
}

void SharedMemoryApp::appB()
{
    while(!m_exitFlag.load())
    {
        if (isSharedMemoryEmpty())
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if(!m_dataQueue.empty())
            {
                sendDataToSharedMemory(m_dataQueue.front());
                m_dataQueue.pop();
            }
            else
            {

            }
        }
        else
        {
            // std::cerr << m_shmaddr << std::endl;
        }
    }
}

void SharedMemoryApp::run()
{
    std::cout << "App SharedMemory is running..." << std::endl;

    signal(SIGINT, [](int) {
        std::cout << "Exiting..." << std::endl;
        app.setExitFlag(); // 종료 플래그 설정
    });

    UDPinit();
    SharedMemoryinit();

    // appA();

    // appA를 쓰레드로 실행
    std::thread threadA(&SharedMemoryApp::appA, this);

    // appB를 쓰레드로 실행
    std::thread threadB(&SharedMemoryApp::appB, this);

    // 스레드 대기
    threadA.join();
    threadB.join();
}

int main()
{
    app.run();

    return 0;
}