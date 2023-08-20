#include "output.h"

OutputApp app;

void OutputApp::setExitFlag() {
    m_exitFlag.store(true);
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
    std::cerr << "shared memory id:" << m_shmid << std::endl;
    m_shmaddr = (char *)shmat(m_shmid, (void *)0, 0);

    std::cerr << "shared memory init" << std::endl;
}

bool OutputApp::isSharedMemoryEmpty() {
    return m_shmaddr[0] == '\0';
}

std::string OutputApp::loadDataFromSharedMemory()
{
    std::string data;
    // 공유 메모리에서 데이터 읽어오는 코드
    if(!isSharedMemoryEmpty())
    {
        data = m_shmaddr;
        // 공유 메모리 초기화 (데이터를 읽은 후)
        memset(m_shmaddr, 0x00, 1024);
    }
    else
    {
        // std::cerr << "????" << std::endl;
    }

    return data;
}

void OutputApp::saveDataToFile()
{
    std::ofstream outputFile("received_data.txt", std::ios::app);
    if (outputFile.is_open())
    {
        while (!m_dataQueue.empty())
        {
            std::string message = m_dataQueue.front();
            m_dataQueue.pop();
            outputFile << message << std::endl;
        }
        outputFile.close();
        std::cout << "Data saved to received_data.txt" << std::endl;
    }
    else
    {
        std::cerr << "Unable to open the file" << std::endl;
    }
}

void OutputApp::appA()
{
    while (!m_exitFlag.load())
    {
        // 공유 메모리에서 데이터 읽기
        std::string message = loadDataFromSharedMemory();

        if (!message.empty())
        {
            // 데이터가 있는 경우에만 큐에 push
            // std::cerr << message << std::endl;
            m_dataQueue.push(message);
            message.clear();
            saveDataToFile();
        }
        else
        {
            // std::cerr << "data empty" << std::endl;
        }

        // if (!m_dataQueue.empty())
        // {
        //     // 큐에 데이터가 있는 경우에만 파일에 저장
        //     saveDataToFile();
        // }
        // else
        // {
        //     // std::cerr << "data empty" << std::endl;
        // }
    }

        // 공유 메모리 초기화
        // memset(m_shmaddr, 0, 1024);
}

void OutputApp::run()
{
    std::cout << "App Output is running..." << std::endl;

    signal(SIGINT, [](int) {
        std::cout << "Exiting..." << std::endl;
        app.setExitFlag(); // 종료 플래그 설정
    });

    Outputinit();

    appA();
}

int main()
{
    app.run();

    return 0;
}