#include "tcpSocket.h"

#define QUEUE_NAME "/my_message_queue"
#define MAX_MSG_SIZE 1024

TcpSocketApp app;

void TcpSocketApp::setExitFlag() {
    m_exitFlag.store(true);
    mq_close(app.getMQdata());
    close(app.getTCPClient());
}

int TcpSocketApp::getMQdata()
{
    return m_mq;
}

int TcpSocketApp::getTCPClient()
{
    return m_client_fd;
}

TcpSocketApp::TcpSocketApp()
{
    m_mq = -1;
    m_client_fd = -1;
}

TcpSocketApp::~TcpSocketApp()
{
    if (m_mq != -1)
    {
        mq_close(m_mq);
        mq_unlink(QUEUE_NAME);
    }
    if (m_client_fd != -1)
    {
        close(m_client_fd);
    }
}

void TcpSocketApp::MQinit()
{
    // 메시지 큐 열기
    m_mq = mq_open(QUEUE_NAME, O_RDONLY);
    if (m_mq == -1)
    {
        perror("mq_open");
        // return 1;
    }
}

void TcpSocketApp::TCPinit()
{
    /* TCP */
    // 소켓 생성
    if ((m_client_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        // return 1;
    }

    m_server_address.sin_family = AF_INET;
    m_server_address.sin_port = htons(12345);

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
        // return 1;
    }
    std::cerr << "TCP Connected" << std::endl;
}

void TcpSocketApp::appA()
{
    while (!m_exitFlag.load())
    {
        memset(m_mqBuffer, 0x00, sizeof(m_mqBuffer));
        // 메시지 받기
        ssize_t bytes_read = mq_receive(m_mq, m_mqBuffer, MAX_MSG_SIZE, nullptr);
        if (bytes_read == -1)
        {
            perror("mq_receive");
            sleep(1);
            // return 1;
        }
        else
        {
            // 메시지 전송
            if (send(m_client_fd, m_mqBuffer, sizeof(m_mqBuffer), 0) == -1)
                std::cout << "send error occur" << std::endl;

            else
                std::cout << "send message to tcp: " << m_mqBuffer << std::endl;
            // std::cout << "Message sent to unixDomainTask app: " << receivedMessage << std::endl;
        }
    }
}

void TcpSocketApp::run()
{
    signal(SIGINT, [](int) {
        std::cout << "Exiting..." << std::endl;
        app.setExitFlag(); // 종료 플래그 설정
    });

    MQinit();
    TCPinit();

    appA();
}

int main()
{
    std::cout << "App TCP is running..." << std::endl;

    app.run();

    return 0;
}