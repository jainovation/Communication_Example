#include "output.h"

OutputApp app;

void OutputApp::setExitFlag() {
    m_exitFlag.store(true);
    shmdt(m_shmaddr);
    close(m_client_fd);
}

OutputApp::OutputApp()
{
    m_shm_key = 0;
    m_shmid = -1;
    m_shmaddr = nullptr;
}

OutputApp::~OutputApp()
{
    if (m_shmaddr != nullptr)
    {
        shmdt(m_shmaddr);
    }
    if (m_client_fd != -1 )
    {
        close(m_client_fd);
    }
}

void OutputApp::Outputinit()
{
    // 공유 메모리 키 생성
    m_shm_key = ftok("shared_memory_key", 65);

    // 공유 메모리 세그먼트에 연결
    m_shmid = shmget(m_shm_key, SHARED_MEM_SIZE, 0666);
    if (m_shmid == -1) {
        perror("shmget");
        throw m_shmid;
    }
    else
    {
        std::cerr << "shared memory id:" << m_shmid << std::endl;
    }
    m_shmaddr = (Message_t *)shmat(m_shmid, NULL, 0);

    // 클라이언트가 작업을 완료했음을 서버에 알리기 위해 -1을 쓴다.
    *m_shmaddr->header = -1;

    std::cerr << "shared memory init Done" << std::endl;
}

void OutputApp::TCPinit()
{
    /* TCP */
    // 소켓 생성
    if ((m_client_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        // return 1;
    }

    m_server_address.sin_family = AF_INET;
    m_server_address.sin_port = htons(45678);

    // IP 주소 설정
    if (inet_pton(AF_INET, "127.0.0.1", &m_server_address.sin_addr) <= 0)
    {
        perror("Invalid address/ Address not supported");
        // return 1;
    }

    // 서버에 연결
    while (connect(m_client_fd, (struct sockaddr *)&m_server_address, sizeof(m_server_address)) < 0)
    // if (connect(m_client_fd, (struct sockaddr *)&m_server_address, sizeof(m_server_address)) < 0)
    {
        perror("Connection failed");
        sleep(3);
    }
    std::cerr << "TCP Connected" << std::endl;
}

bool OutputApp::isSharedMemoryEmpty()
{
    // return m_shmaddr[0] == '\0';
    return true;
}

std::string OutputApp::loadDataFromSharedMemory()
{
    std::string data;
    // 공유 메모리에서 데이터 읽어오는 코드
    if(!isSharedMemoryEmpty())
    {
        // data = m_shmaddr;
        // 공유 메모리 초기화 (데이터를 읽은 후)
        memset(m_shmaddr, 0x00, 1024);
    }
    else
    {
        // std::cerr << "????" << std::endl;
    }

    return data;
}

void OutputApp::appA()
{
    char response[1] = {0};

    while (!m_exitFlag.load())
    {
        ssize_t bytesRead = read(m_client_fd, response, sizeof(response));
        if (bytesRead == -1)
        {
            std::cerr << "Error: Unable to receive message" << std::endl;
            throw false;
        }
        else
        {
            // 공유 메모리에서 데이터 읽기
            // memcpy(&m_data, m_shmaddr, sizeof(m_data));
            // while(!(strlen(m_data.data) == atoi(m_data.header)))
            // {
            //     memcpy(&m_data, m_shmaddr, sizeof(m_data));
            // }
            saveDataToFile();
            memset(m_shmaddr, 0x00, SHARED_MEM_SIZE);
        }
        send(m_client_fd, response, sizeof(response), 0);

    }
}

void OutputApp::saveDataToFile()
{
    std::ofstream outputFile("received_data.txt", std::ios::app);
    if (outputFile.is_open())
    {
        // 뮤텍스 락
        // std::unique_lock<std::mutex> lock(m_mutex);
        // std::lock_guard<std::mutex> lock(m_mutex);
        // m_mutex.lock();
        outputFile << m_shmaddr->data << std::endl;
        // outputFile << m_data.data << std::endl;
        // m_mutex.unlock();

        outputFile.close();
        std::cout << "Data saved to received_data.txt" << std::endl;
    }
    else
    {
        std::cerr << "Unable to open the file" << std::endl;
    }
}

void OutputApp::run()
{
    std::cout << "App Output is running..." << std::endl;

    signal(SIGINT, [](int) {
        std::cout << "Exiting..." << std::endl;
        app.setExitFlag(); // 종료 플래그 설정
    });

    Outputinit();
    TCPinit();

    appA();
}

int main()
{
    app.run();

    return 0;
}