#ifndef MESSAGEQUEUEAPP_H
#define MESSAGEQUEUEAPP_H

#include <mqueue.h>

#define QUEUE_NAME "/my_message_queue"
#define MAX_MSG_SIZE 1024

class MessageQueueApp
{
public:
    MessageQueueApp();
    ~MessageQueueApp();
    int getMQdata();
    void run();

private:
    mqd_t m_mq;

    void appA();
};

#endif // MESSAGEQUEUEAPP_H