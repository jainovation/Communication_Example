#ifndef MESSAGEQUEUEAPP_H
#define MESSAGEQUEUEAPP_H

#include <mqueue.h>
#include <atomic>
#include "../include/common.h"

class MessageQueueApp
{
public:
    MessageQueueApp();
    ~MessageQueueApp();
    int getMQdata();
    void run();
    void setExitFlag(); // 종료 플래그 설정 함수
    bool sendMessage(const Message_t& message);


private:
    mqd_t m_mq;
    std::atomic<bool> m_exitFlag; // 종료 플래그

    void appA();

};

#endif // MESSAGEQUEUEAPP_H