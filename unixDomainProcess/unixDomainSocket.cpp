#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <chrono>
#include <sys/un.h>
#include "../include/common.h"

#define SOCKET_PATH "/tmp/my_unix_socket"

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
        std::cout << "Received message from tcpTask: " << g_tcpBuffer << std::endl;
        std::cout << "cnt: " << cnt++ << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_DELAY * 10));
    }
}

void appB()
{
    int server_socket;
    struct sockaddr_un server_addr;

    // 소켓 생성
    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("socket");
        // return 1;
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // 소켓 바인딩
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un)) == -1)
    {
        perror("bind");
        close(server_socket);
        // return 1;
    }

    // 클라이언트 연결 대기
    if (listen(server_socket, 5) == -1)
    {
        perror("listen");
        close(server_socket);
        // return 1;
    }

    // 클라이언트 연결 수락
    int client_socket = accept(server_socket, NULL, NULL);
    if (client_socket == -1)
    {
        perror("accept");
        close(server_socket);
        // return 1;
    }

    while (true)
    {
        // 데이터 전송
        const char *message = g_tcpBuffer;
        send(client_socket, message, strlen(message), 0);

        std::cout << "Message sent to UDPTask: " << message << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_DELAY * 10));
    }
    // 연결 종료
    close(client_socket);
    close(server_socket);
}

int main()
{
    // App A 작업 수행
    std::cout << "App A is running..." << std::endl;
    // App B 작업 수행
    std::cout << "App B is running..." << std::endl;

    std::thread threadA(appA);
    std::thread threadB(appB);

    // 스레드 대기
    threadA.join();
    threadB.join();

    return 0;
}