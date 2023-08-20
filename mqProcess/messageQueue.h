#ifndef MESSAGEQUEUEAPP_H
#define MESSAGEQUEUEAPP_H

#include <mqueue.h>
#include <atomic>

#define QUEUE_NAME "/my_message_queue"
#define MAX_MSG_SIZE 1024

class MessageQueueApp
{
public:
    MessageQueueApp();
    ~MessageQueueApp();
    int getMQdata();
    void run();
    void setExitFlag(); // 종료 플래그 설정 함수

private:
    mqd_t m_mq;
    std::atomic<bool> m_exitFlag; // 종료 플래그

    void appA();

};

#endif // MESSAGEQUEUEAPP_H