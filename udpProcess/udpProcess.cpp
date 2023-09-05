#include "udpProcess.h"

UdpSocketApp app;

void UdpSocketApp::setExitFlag() {
    m_exitFlag.store(true);
    close(m_client_socket);
    close(m_udp_socket);
    // close(m_server_socket);
}

UdpSocketApp::UdpSocketApp()
{
    m_client_socket = -1;
    m_udp_socket = -1;
}

UdpSocketApp::~UdpSocketApp()
{
    if (m_client_socket != -1)
    {
        close(m_client_socket);
    }
    if (m_udp_socket != -1)
    {
        close(m_udp_socket);
    }
}

void UdpSocketApp::Unixinit()
{
    // 소켓 생성
    m_client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (m_client_socket == -1)
    {
        perror("socket");
        // return 1;
    }

    memset(&m_server_addr, 0, sizeof(struct sockaddr_un));
    m_server_addr.sun_family = AF_UNIX;
    strncpy(m_server_addr.sun_path, SOCKET_PATH, sizeof(m_server_addr.sun_path) - 1);

    // 서버에 연결
    while (connect(m_client_socket, (struct sockaddr *)&m_server_addr, sizeof(struct sockaddr_un)) == -1)
    {
        perror("connect failed");
        // close(client_socket);
        sleep(3);
        // return 1;
    }
    std::cerr << "unix Socket Connected" << std::endl;
}

void UdpSocketApp::UDPinit()
{
    // 소켓 생성
    if ((m_udp_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket");
        // return 1;
    }

    memset(&m_server_address, 0, sizeof(m_server_address));

    m_server_address.sin_family = AF_INET;
    m_server_address.sin_port = htons(23456);
    m_server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    m_server_addr_len = sizeof(m_server_address);

    std::cout << "UDP 서버 시작. 포트: " << m_server_address.sin_port << std::endl;

    std::cerr << "udp Socket Connected" << std::endl;
}

bool UdpSocketApp::receiveMessage(Message_t &message)
{
    char receivedMessage[sizeof(Message_t)];

    // 데이터 수신
    ssize_t bytesRead = recv(m_client_socket, receivedMessage, sizeof(receivedMessage), 0);
    if (bytesRead == -1)
    {
        std::cerr << "Error: Unable to receive message" << std::endl;
        return false;
    }

    memcpy(&message, receivedMessage, sizeof(message));

    return true;
}

bool UdpSocketApp::sendMessage(const Message_t &message)
{
    char sendMessage[sizeof(Message_t)];

    memcpy(&sendMessage, &message, sizeof(sendMessage));

    if (sendto(m_udp_socket, sendMessage, sizeof(sendMessage), 0, (struct sockaddr *)&m_server_address, sizeof(m_server_address)) < 0)
    {
        // std::cout << "send error occur" << std::endl;
        perror("sendto error");
        // return false;
        throw false;
    }
    else
    {
        std::cout << "send message to smTask: " << message.data << std::endl;
    }
    return true;
}

bool UdpSocketApp::retryMessageFromUDP()
{
    // 서버로부터 응답 수신
    int bytes_received = recvfrom(m_udp_socket, m_ackBuffer, sizeof(m_ackBuffer), 0, nullptr, nullptr);
    if (bytes_received < 0)
    {
        perror("Error receiving data");
        throw (int)m_ackBuffer[0];
        // return 1;
    }
    else
    {
        // std::cout << "ack true: " << message.data << std::endl;
    }
    return (int)m_ackBuffer[0];
}

void UdpSocketApp::appA()
{
    bool status = true;

    while (!m_exitFlag.load())
    {
        memset(&m_receivedMessage, 0x00, sizeof(m_receivedMessage));

        // 메시지 받기
        status = receiveMessage(m_receivedMessage);

        if (status == false)
        {
            perror("recv");
            throw false;
        }
        else
        {
            // 데이터 전송
            sendMessage(m_receivedMessage);
            while(retryMessageFromUDP() == true)
            {
                // 데이터 전송
                sendMessage(m_receivedMessage);
            }
        }
        saveDataToFile();
    }
}

void UdpSocketApp::saveDataToFile()
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

void UdpSocketApp::run()
{
    signal(SIGINT, [](int) {
        std::cout << "Exiting..." << std::endl;
        app.setExitFlag(); // 종료 플래그 설정
    });

    Unixinit();
    UDPinit();

    appA();
}

int main()
{
    std::cout << "App UDP is running..." << std::endl;

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