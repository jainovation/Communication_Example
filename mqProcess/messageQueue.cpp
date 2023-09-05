#include "messageQueue.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <signal.h>

MessageQueueApp app;

void MessageQueueApp::setExitFlag() {
    m_exitFlag.store(true);
    mq_close(m_mq);
    mq_unlink(QUEUE_NAME);
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
    // attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_msgsize = sizeof(Message_t);
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

bool MessageQueueApp::sendMessage(const Message_t &message)
{
    char sendMessage[sizeof(Message_t)];

    memcpy(&sendMessage, &message, sizeof(sendMessage));

    if (mq_send(m_mq, sendMessage, sizeof(sendMessage), 0) == -1)
    {
        std::cerr << "Error: Unable to send message" << std::endl;
        // return false;
        throw false;
    }
    return true;
}

void MessageQueueApp::appA()
{
    while (!m_exitFlag.load())
    {
        std::string fileName;
        std::cout << "전송할 파일명을 입력해주세요.: ";
        std::cin >> fileName;

        if (fileName == "q")
        {
            break;
        }

        // 텍스트 파일 읽기
        std::ifstream file(fileName);
        if (!file.is_open())
        {
            std::cerr << "Failed to open input file." << std::endl;
        }

        char buffer[DATA_SIZE];  // 텍스트를 읽을 버퍼
        Message_t message;    // Message 구조체

        memset(&message, 0x00, sizeof(message));

        // 파일에서 텍스트 읽어오기
        while (file.getline(buffer, sizeof(buffer)))
        {
            // 헤더(header) 설정 (텍스트 길이)
            int textLength = strlen(buffer);

            snprintf(message.header, sizeof(message.header), "%d", textLength);

            // 본문(body) 설정
            strncpy(message.data, buffer, sizeof(message.data));

            // 테일(tail) 설정
            strncpy(message.tail, "\n", sizeof(message.tail));

            // 메시지 보내기
            sendMessage(message);
        }
        file.close();
    }
}

void MessageQueueApp::run()
{
    // Ctrl+C로 종료 시그널 처리를 위한 핸들러 등록
    signal(SIGINT, [](int) {
        std::cout << "Exiting..." << std::endl;
        app.setExitFlag(); // 종료 플래그 설정
    });

    appA();
}

int main()
{
    std::cout << "App MQ is running..." << std::endl;

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