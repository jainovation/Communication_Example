#include "tcpSocket.h"

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
    }
    std::cerr << "TCP Connected" << std::endl;
}

bool TcpSocketApp::receiveMessage(Message_t &message)
{
    unsigned int prio;
    char receivedMessage[sizeof(Message_t)];

    ssize_t bytesRead = mq_receive(m_mq, receivedMessage, sizeof(receivedMessage), &prio);
    if (bytesRead == -1)
    {
        std::cerr << "Error: Unable to receive message" << std::endl;
        return false;
    }

    memcpy(&message, receivedMessage, sizeof(message));

    return true;
}

bool TcpSocketApp::sendMessage(const Message_t &message)
{
    char sendMessage[sizeof(Message_t)];

    memcpy(&sendMessage, &message, sizeof(sendMessage));

    if (send(m_client_fd, sendMessage, sizeof(sendMessage), 0) == -1)
    {
        std::cout << "send error occur" << std::endl;
        // return false;
        throw false;
    }
    else
    {
        std::cout << "Message sent to unixDomainTask app: " << message.data << std::endl;
    }
    return true;
}

void TcpSocketApp::appA()
{
    bool status = true;

    while (!m_exitFlag.load())
    {
        memset(&m_receivedMessage, 0x00, sizeof(m_receivedMessage));

        // 메시지 받기
        status = receiveMessage(m_receivedMessage);

        if (status == false)
        {
            perror("mq_receive");
            throw false;
        }
        else
        {
            // 메시지 전송
            sendMessage(m_receivedMessage);
        }
        saveDataToFile();
    }
}

void TcpSocketApp::saveDataToFile()
{
    // 헤더(header), 본문(data), 테일(tail) 추출
    int textLength = atoi(m_receivedMessage.header);
    std::string data(m_receivedMessage.data, textLength);

    std::ofstream outputFile("received_data.txt", std::ios::app);
    if (outputFile.is_open())
    {
        outputFile << data << std::endl;

        outputFile.close();
        // std::cout << "Data saved to received_data.txt" << std::endl;
    }
    else
    {
        std::cerr << "Unable to open the file" << std::endl;
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