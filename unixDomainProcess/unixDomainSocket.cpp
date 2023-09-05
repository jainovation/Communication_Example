#include "unixDomainSocket.h"

UnixSocketApp app;

void UnixSocketApp::setExitFlag() {
    m_exitFlag.store(true);
    close(m_client_socket);
    close(m_server_socket);
    close(m_new_socket);
    close(m_server_fd);
    unlink(SOCKET_PATH);
}

void UnixSocketApp::Unixinit()
{
    unlink(SOCKET_PATH);
    // 소켓 생성
    m_server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (m_server_socket == -1)
    {
        perror("socket");
        // return 1;
    }

    memset(&m_server_addr, 0, sizeof(struct sockaddr_un));
    m_server_addr.sun_family = AF_UNIX;
    strncpy(m_server_addr.sun_path, SOCKET_PATH, sizeof(m_server_addr.sun_path) - 1);

    // 소켓 바인딩
    if (bind(m_server_socket, (struct sockaddr *)&m_server_addr, sizeof(struct sockaddr_un)) == -1)
    {
        perror("bind");
        // close(m_server_socket);
        // return 1;
    }

    // 클라이언트 연결 대기
    if (listen(m_server_socket, 5) == -1)
    {
        perror("listen");
        // close(m_server_socket);
        // return 1;
    }

    // 클라이언트 연결 수락
    m_client_socket = accept(m_server_socket, NULL, NULL);
    if (m_client_socket == -1)
    {
        perror("accept");
        // close(m_server_socket);
        // return 1;
    }
    std::cerr << "Unix Connected" << std::endl;
}

void UnixSocketApp::TCPinit()
{
    int addrlen = sizeof(m_address);

    // 소켓 생성
    if ((m_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        // return 1;
    }

    m_address.sin_family = AF_INET;
    m_address.sin_addr.s_addr = INADDR_ANY;
    m_address.sin_port = htons(12345);

    // 바인딩
    if (bind(m_server_fd, (struct sockaddr *)&m_address, sizeof(m_address)) < 0)
    {
        perror("bind failed");
        // return 1;
    }

    // 수신 대기
    if (listen(m_server_fd, 3) < 0)
    {
        perror("listen failed");
        // return 1;
    }

    // 클라이언트 연결 대기 및 처리
    if ((m_new_socket = accept(m_server_fd, (struct sockaddr *)&m_address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept failed");
        // return 1;
    }
    std::cerr << "TCP Connected" << std::endl;
}

UnixSocketApp::UnixSocketApp()
{
     m_server_fd = -1;
     m_new_socket = -1;
     m_server_socket = -1;
     m_client_socket = -1;
}

UnixSocketApp::~UnixSocketApp()
{
    if (m_new_socket != -1)
    {
        close(m_new_socket);
    }
    if (m_server_fd != -1)
    {
        close(m_server_fd);
    }
    if (m_server_socket != 1)
    {
        close(m_server_socket);
    }
    if (m_client_socket = -1)
    {
        close(m_client_socket);
        unlink(SOCKET_PATH);
    }
}

bool UnixSocketApp::receiveMessage(Message_t &message)
{
    char receivedMessage[sizeof(Message_t)];

    ssize_t bytesRead = read(m_new_socket, receivedMessage, sizeof(receivedMessage));
    if (bytesRead == -1)
    {
        std::cerr << "Error: Unable to receive message" << std::endl;
        return false;
    }

    memcpy(&message, receivedMessage, sizeof(message));

    return true;
}

bool UnixSocketApp::sendMessage(const Message_t &message)
{
    char sendMessage[sizeof(Message_t)];

    memcpy(&sendMessage, &message, sizeof(sendMessage));

    if (send(m_client_socket, sendMessage, sizeof(sendMessage), 0) == -1)
    {
        std::cout << "send error occur" << std::endl;
        // return false;
        throw false;
    }
    else
    {
        std::cout << "send message to udpTask: " << message.data << std::endl;
    }
    return true;
}

void UnixSocketApp::appA()
{
    bool status = true;

    while (!m_exitFlag.load())
    {
        memset(&m_receivedMessage, 0x00, sizeof(m_receivedMessage));

        // 메시지 받기
        status = receiveMessage(m_receivedMessage);

        if (status == false)
        {
            perror("tcp_receive");
            throw false;
        }
        else
        {
            sendMessage(m_receivedMessage);
        }
        saveDataToFile();
    }
}

void UnixSocketApp::saveDataToFile()
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

void UnixSocketApp::run()
{
    signal(SIGINT, [](int) {
        std::cout << "Exiting..." << std::endl;
        app.setExitFlag(); // 종료 플래그 설정
    });

    TCPinit();
    Unixinit();

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