#include <iostream>
#include <fstream>
#include <cstring>
#include <mqueue.h>
#include <thread>
#include <chrono>
#include "../include/common.h"

#define QUEUE_NAME "/my_message_queue"
#define MAX_MSG_SIZE 1024

//********************************//
//                                //
// MQ -> TCP -> UNIX -> UDP -> SM //
//                                //
//********************************//

void appA()
{
    // 메시지 큐 생성
    mqd_t mq;
    struct mq_attr attr;
    int cnt = 0;

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;            // 최대 메시지 개수
    attr.mq_msgsize = MAX_MSG_SIZE; // 메시지 최대 크기
    attr.mq_curmsgs = 0;

    while (true)
    {
        mq = mq_open(QUEUE_NAME, O_WRONLY | O_CREAT, 0644, &attr);
        if (mq == -1)
        {
            perror("mq_open");
            // return 1;
        }

        // 파일에서 문자 읽기
        std::ifstream file("input.txt");
        if (!file.is_open())
        {
            std::cerr << "Failed to open input file." << std::endl;
            // return 1;
        }

        std::string message;
        std::getline(file, message);
        file.close();

        // 메시지 전송
        if (mq_send(mq, message.c_str(), message.size() + 1, 0) == -1)
        {
            perror("mq_send");
            // return 1;
        }

        std::cout << "Message sent to queue: " << message << std::endl;
        std::cout << "cnt: " << cnt++ << std::endl;

        mq_close(mq);

        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_DELAY * 10));
    }
    mq_unlink(QUEUE_NAME);

}

int main()
{
    // App 작업 수행
    std::cout << "App MQ is running..." << std::endl;

    std::thread threadA(appA);

    // 스레드 대기
    threadA.join();

    return 0;
}