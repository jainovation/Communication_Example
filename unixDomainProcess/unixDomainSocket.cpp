#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <chrono>

char g_tcpBuffer[1024];

void appA()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    // char tcpBuffer[1024] = {0};
    int cnt = 0;

    // 소켓 생성
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        // return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    // 바인딩
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        // return 1;
    }

    // 수신 대기
    if (listen(server_fd, 3) < 0)
    {
        perror("listen failed");
        // return 1;
    }

    // 클라이언트 연결 대기 및 처리
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept failed");
        // return 1;
    }

    while (true)
    {

        // 클라이언트로부터 데이터 읽기
        read(new_socket, g_tcpBuffer, 1024);
        std::cout << "Received message from tcpTask app: " << g_tcpBuffer << std::endl;
        std::cout << "cnt: " << cnt++ << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

#if 0
void appB()
{
    int udp_socket;
    struct sockaddr_in server_address;
    // char buffer[1024];

    // 소켓 생성
    if ((udp_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket");
        // return 1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8081);
    server_address.sin_addr.s_addr = inet_addr("172.24.158.228");

    while (true)
    {
        // 앱에서 메시지 받기
        std::string receivedMessage = g_tcpBuffer;

        // 데이터 전송
        ssize_t bytes_sent = sendto(udp_socket, receivedMessage, strlen(receivedMessage), 0, (struct sockaddr *)&server_address, sizeof(server_address));
        if (bytes_sent == -1)
        {
            perror("sendto");
            // return 1;
        }

        std::cout << "Message sent to udpTask app" << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}
#endif

int main()
{

    // App A 작업 수행
    std::cout << "App A is running..." << std::endl;
    // App B 작업 수행
    // std::cout << "App B is running..." << std::endl;

    std::thread threadA(appA);
    // std::thread threadB(appB);

    // 스레드 대기
    threadA.join();
    // threadB.join();

    return 0;
}