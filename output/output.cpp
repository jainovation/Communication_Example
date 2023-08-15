#include "output.h"

OutputApp app;

static void exitHandler(int signalNum)
{
    if (signalNum == SIGINT || signalNum == SIGTERM || signalNum == SIGKILL)
    {
        // close(m_client_socket);
        // close(m_server_socket);
    }
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
}

void OutputApp::Outputinit()
{
    // 공유 메모리 연결
    m_shm_key = ftok("shared_memory_key", 65); // 키 생성
    m_shmid = shmget(m_shm_key, 1024, 0666);
    m_shmaddr = (char *)shmat(m_shmid, (void *)0, 0);

    std::cerr << "shared memory init" << std::endl;
}

void OutputApp::appA()
{
    while (true)
    {
        // 데이터 읽기
        std::string message = m_shmaddr;

        // 데이터 수신 여부 확인
        if (!message.empty())
        {
            // 텍스트 파일로 저장
            std::ofstream outputFile("received_data.txt");
            if (outputFile.is_open())
            {
                outputFile << message;
                outputFile.close();
                std::cout << "Data saved to received_data.txt" << std::endl;
            }
            else
            {
                std::cerr << "Unable to open the file" << std::endl;
            }
        }
        else
        {
        }
        // 공유 메모리 초기화
        memset(m_shmaddr, 0, 1024);
    }
}

void OutputApp::run()
{
    std::cout << "App Output is running..." << std::endl;

    signal(SIGINT, exitHandler);
    signal(SIGTERM, exitHandler);
    signal(SIGKILL, exitHandler);

    Outputinit();

    appA();
}

int main()
{
    app.run();

    return 0;
}