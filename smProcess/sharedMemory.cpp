#include <iostream>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <thread>
#include <chrono>

#include <fstream>

void appA()
{
    key_t key = ftok("shared_memory_key", 65); // 키 생성

    // 공유 메모리 연결
    int shmid = shmget(key, 1024, 0666);
    char *shmaddr = (char *)shmat(shmid, (void *)0, 0);

    while (true)
    {
        // 데이터 읽기
        std::string message = shmaddr;
        std::cout << "Received message from udpTask: " << message << std::endl;

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

        // 공유 메모리 삭제 (옵션)
        // shmctl(shmid, IPC_RMID, NULL);

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    // 연결 해제
    shmdt(shmaddr);
}

int main()
{
    // App A 작업 수행
    std::cout << "App A is running..." << std::endl;

    std::thread threadA(appA);

    // 스레드 대기
    threadA.join();

    return 0;
}