#include "udpProcess.h"

#define SOCKET_PATH "/tmp/my_unix_socket"

UdpSocketApp app;

static void exitHandler(int signalNum)
{
    if (signalNum == SIGINT || signalNum == SIGTERM || signalNum == SIGKILL)
    {
        // close(m_client_socket);
        // close(m_server_socket);
    }
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

    m_server_address.sin_family = AF_INET;
    m_server_address.sin_port = htons(23456);
    m_server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    std::cerr << "udp Socket Connected" << std::endl;
}

void UdpSocketApp::appA()
{
    while (true)
    {
        memset(m_unixBuffer, 0x00, sizeof(m_unixBuffer));

        // 데이터 수신
        ssize_t bytes_received = recv(m_client_socket, m_unixBuffer, sizeof(m_unixBuffer), 0);
        if (bytes_received == -1)
        {
            perror("recv");
            sleep(1);
            // close(client_socket);
            // return 1;
        }
        else
        {
            // 수신된 데이터 출력
            // m_unixBuffer[bytes_received] = '\0';
            // usleep(10);
            // 데이터 전송
            ssize_t bytes_sent = sendto(m_udp_socket, m_unixBuffer, sizeof(m_unixBuffer), 0, (struct sockaddr *)&m_server_address, sizeof(m_server_address));
            if (bytes_sent == -1)
            {
                perror("sendto");
                sleep(1);
                // return 1;
            }
            else
            {
                std::cout << "Received message from unixDomainTask: " << m_unixBuffer << std::endl;
            }
        }
    }
}

void UdpSocketApp::run()
{
    signal(SIGINT, exitHandler);
    signal(SIGTERM, exitHandler);
    signal(SIGKILL, exitHandler);

    Unixinit();
    UDPinit();

    appA();
}

int main()
{
    std::cout << "App UDP is running..." << std::endl;

    app.run();

    return 0;
}