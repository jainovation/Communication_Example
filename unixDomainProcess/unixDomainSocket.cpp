#include "unixDomainSocket.h"

UnixSocketApp app;

static void exitHandler(int signalNum)
{
    if (signalNum == SIGINT || signalNum == SIGTERM || signalNum == SIGKILL)
    {
        // close(m_client_socket);
        // close(m_server_socket);
    }
}

void UnixSocketApp::Unixinit()
{
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

void UnixSocketApp::appA()
{
    while (true)
    {
        // 클라이언트로부터 데이터 읽기
        memset(m_tcpBuffer, 0x00, sizeof(m_tcpBuffer));

        if(read(m_new_socket, m_tcpBuffer, 1024) == -1)
        {
            perror("mq_receive");
            sleep(1);
        }
        else
        {
            usleep(10);
            if (send(m_client_socket, m_tcpBuffer, strlen(m_tcpBuffer), 0) == -1)
                std::cout << "send error occur" << std::endl;
            else
                std::cout << "Received message from tcpTask: " << m_tcpBuffer << std::endl;
        }
    }
}

void UnixSocketApp::run()
{
    signal(SIGINT, exitHandler);
    signal(SIGTERM, exitHandler);
    signal(SIGKILL, exitHandler);

    TCPinit();
    Unixinit();

    appA();
}

int main()
{
    std::cout << "App TCP is running..." << std::endl;

    app.run();

    return 0;
}