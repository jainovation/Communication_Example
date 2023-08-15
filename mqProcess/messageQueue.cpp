#include "messageQueue.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <signal.h>
#include "../include/common.h"

MessageQueueApp app;

static void exitHandler(int signalNum)
{
    if (signalNum == SIGINT || signalNum == SIGTERM || signalNum == SIGKILL)
    {
        mq_close(app.getMQdata());
        mq_unlink(QUEUE_NAME);
    }
}

int MessageQueueApp::getMQdata()
{
    return m_mq;
}

MessageQueueApp::MessageQueueApp()
{
    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    m_mq = mq_open(QUEUE_NAME, O_WRONLY | O_CREAT, 0644, &attr);
    if (m_mq == -1)
    {
        perror("mq_open");
    }
}

MessageQueueApp::~MessageQueueApp()
{
    mq_close(m_mq);
    mq_unlink(QUEUE_NAME);
}

void MessageQueueApp::appA()
{
    while (true)
    {
        std::string fileName;
        std::cout << "전송할 파일명을 입력해주세요.: ";
        std::cin >> fileName;

        if (fileName == "q")
        {
            break;
        }

        std::ifstream file(fileName);
        if (!file.is_open())
        {
            std::cerr << "Failed to open input file." << std::endl;
        }

        std::string message;
        while (getline(file, message))
        {
            if (mq_send(m_mq, message.c_str(), message.size() + 1, 0) == -1)
            {
                perror("mq_send");
            }
            std::cout << "Message sent to queue: " << message << std::endl;
        }
        file.close();
    }
}

void MessageQueueApp::run()
{
    signal(SIGINT, exitHandler);
    signal(SIGTERM, exitHandler);
    signal(SIGKILL, exitHandler);

    appA();
}

int main()
{
    std::cout << "App MQ is running..." << std::endl;

    app.run();

    return 0;
}