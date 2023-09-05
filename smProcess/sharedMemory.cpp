#include "sharedMemory.h"

SharedMemoryApp app;

void SharedMemoryApp::setExitFlag() {
    m_exitFlag.store(true);
    close(m_udp_socket);
    close(m_new_socket);
    close(m_server_fd);
    shmdt(m_shmaddr);
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

    if (m_new_socket != -1)
    {
        close(m_new_socket);
    }
    if (m_server_fd != -1)
    {
        close(m_server_fd);
    }
}

void SharedMemoryApp::SharedMemoryinit()
{
    m_shm_key = ftok("shared_memory_key", 65); // 키 생성

    // 공유 메모리 생성 및 연결
    m_shmid = shmget(m_shm_key, SHARED_MEM_SIZE, 0666 | IPC_CREAT);
    if (m_shmid == -1) {
        perror("shmget");
        throw m_shmid;
    }
    else
    {
        std::cerr << "shared memory id:" << m_shmid << std::endl;
    }
    // m_shmaddr = (char *)shmat(m_shmid, NULL, 0);
    m_shmaddr = (Message_t *)shmat(m_shmid, NULL, 0);

    memset(m_shmaddr, 0x00, SHARED_MEM_SIZE); // 공유 메모리 초기화

    // 클라이언트가 연결하기를 기다립니다.
    std::cout << "SharedMemory 클라이언트가 연결될 때까지 기다립니다." << std::endl;
    while (m_shmaddr->header == NULL)
    {
        // 클라이언트가 값을 쓸 때까지 대기
    }

    std::cerr << "shared memory init Done" << std::endl;
}

void SharedMemoryApp::UDPinit()
{
    // 소켓 생성
    if ((m_udp_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket");
        // return 1;
    }

    memset(&m_server_address, 0, sizeof(m_server_address));

    m_server_address.sin_family = AF_INET;
    m_server_address.sin_addr.s_addr = INADDR_ANY;
    m_server_address.sin_port = htons(23456); // 포트 번호

    std::cout << "UDP 서버 시작. 포트: " << m_server_address.sin_port << std::endl;

    // 소켓 바인딩
    if (bind(m_udp_socket, (struct sockaddr *)&m_server_address, sizeof(m_server_address)) == -1)
    {
        perror("bind");
        // return 1;
    }

    m_client_address_len = sizeof(m_client_address);

    std::cerr << "udp Socket Connected" << std::endl;
}

void SharedMemoryApp::TCPinit()
{
    int addrlen = sizeof(m_address);

    // 소켓 생성
    if ((m_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        // return 1;
    }

    m_address.sin_family = AF_INET;
    m_address.sin_addr.s_addr = INADDR_ANY;
    m_address.sin_port = htons(45678);

    // 바인딩
    if (bind(m_server_fd, (struct sockaddr *)&m_address, sizeof(m_address)) < 0)
    {
        perror("bind failed");
        // return 1;
    }

    // 수신 대기
    if (listen(m_server_fd, 3) < 0)
    {
        perror("listen failed");
        // return 1;
    }

    // 클라이언트 연결 대기 및 처리
    if ((m_new_socket = accept(m_server_fd, (struct sockaddr *)&m_address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept failed");
        // return 1;
    }
    std::cerr << "TCP Connected" << std::endl;
}

void SharedMemoryApp::sendDataToSharedMemory(const std::string &data)
{
    // std::lock_guard<std::mutex> lock(m_mutex);
    // 데이터 길이가 공유 메모리 크기보다 작을 경우에만 쓰기
    // if (data.size() < SHARED_MEM_SIZE) {
        // memset(m_shmaddr, 0x00, 1024); // 공유 메모리 초기화
        // strcpy(m_shmaddr, data.c_str()); // 데이터 쓰기
    // }
}

bool SharedMemoryApp::isSharedMemoryEmpty()
{
    // return m_shmaddr[0] == '\0';
    return 0;
}

bool SharedMemoryApp::receiveMessage(Message_t &message)
{
    char receivedMessage[sizeof(Message_t)];

    // 데이터 수신
    ssize_t bytesRead = recvfrom(m_udp_socket, receivedMessage, sizeof(receivedMessage), 0, (struct sockaddr *)&m_client_address, &m_client_address_len);
    if (bytesRead == -1)
    {
        std::cerr << "Error: Unable to receive message" << std::endl;
        return false;
    }

    memcpy(&message, receivedMessage, sizeof(message));

    std::cout << "received message to udpTask: " << message.data << std::endl;

    return true;
}

bool SharedMemoryApp::checkRetryMessage(Message_t& message)
{
    char response[1] = { true };
    char receivedMessage[sizeof(Message_t)];

    while(!(strlen(message.data) == atoi(message.header)))
    {
        sendto(m_udp_socket, response, sizeof(char), 0, (struct sockaddr*)&m_client_address, m_client_address_len);
        recvfrom(m_udp_socket, receivedMessage, sizeof(receivedMessage), 0, (struct sockaddr *)&m_client_address, &m_client_address_len);
        memcpy(&message, receivedMessage, sizeof(message));
    }
    response[0] = false;
    sendto(m_udp_socket, response, strlen(response), 0, (struct sockaddr*)&m_client_address, m_client_address_len);

    return true;
}

void SharedMemoryApp::appA()
{
    bool status = true;

    char response[1] = {0};

    while (!m_exitFlag.load())
    {
        memset(&m_receivedMessage, 0x00, sizeof(m_receivedMessage));

        // 메시지 받기
        status = receiveMessage(m_receivedMessage);

        if (status == false)
        {
            perror("recv");
            throw false;
        }
        else
        {
            checkRetryMessage(m_receivedMessage);

            strncpy(m_shmaddr->data, m_receivedMessage.data, sizeof(m_shmaddr->data));
            // 데이터의 길이가 공유 메모리 크기보다 큰 경우 실패
            if (strlen(m_receivedMessage.data) >= SHARED_MEM_SIZE)
            {
                std::cerr << "Data size exceeds shared memory size" << std::endl;
                throw strlen(m_receivedMessage.data);
            }

            if (send(m_new_socket, response, sizeof(response), 0) == -1)
            {
                std::cout << "send error occur" << std::endl;
                // return false;
                throw false;
            }
            else
            {
                // 뮤텍스 락
                // std::unique_lock<std::mutex> lock(m_mutex);
                // std::lock_guard<std::mutex> lock(m_mutex);
                // m_mutex.lock();
                // memset(m_shmaddr, 0x00, SHARED_MEM_SIZE);
                // strncpy(m_shmaddr->header, m_receivedMessage.header, sizeof(m_shmaddr->data));

                // memcpy(m_shmaddr->data, m_receivedMessage.data, sizeof(m_shmaddr->data));
                // m_mutex.unlock();
            }

            ssize_t bytesRead = read(m_new_socket, response, sizeof(response));
            if (bytesRead == -1)
            {
                std::cerr << "Error: Unable to receive message" << std::endl;
                throw false;
            }
            else
            {

            }
        }
        saveDataToFile();
    }
}

void SharedMemoryApp::saveDataToFile()
{
    // 헤더(header), 본문(data), 테일(tail) 추출
    int textLength = atoi(m_receivedMessage.header);
    std::string data(m_receivedMessage.data, textLength);

    std::ofstream outputFile("received_data.txt", std::ios::app);
    if (outputFile.is_open())
    {
        outputFile << data << std::endl;

        outputFile.close();
        // std::cout << "Data saved to received_data.txt" << std::endl;
    }
    else
    {
        std::cerr << "Unable to open the file" << std::endl;
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

    TCPinit();

    appA();
}

int main()
{
    try
    {
        app.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}