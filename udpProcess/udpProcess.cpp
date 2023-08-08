#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <thread>
#include <chrono>
#include "../include/common.h"

#define SOCKET_PATH "/tmp/my_unix_socket"

char g_unixBuffer[1024];

void appA()
{
    int client_socket;
    struct sockaddr_un server_addr;
    int cnt = 0;

    // 소켓 생성
    client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("socket");
        // return 1;
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // 서버에 연결
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un)) == -1)
    {
        perror("connect");
        close(client_socket);
        // return 1;
    }

    while (true)
    {
        // 데이터 수신
        // char buffer[1024];
        ssize_t bytes_received = recv(client_socket, g_unixBuffer, sizeof(g_unixBuffer), 0);
        if (bytes_received == -1)
        {
            perror("recv");
            close(client_socket);
            // return 1;
        }

        // 수신된 데이터 출력
        g_unixBuffer[bytes_received] = '\0';
        std::cout << "Received message from unixDomainTask: " << g_unixBuffer << std::endl;
        std::cout << "cnt: " << cnt++ << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_DELAY * 10));
    }
    // 연결 종료
    close(client_socket);
}

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
    server_address.sin_addr.s_addr = inet_addr("172.24.144.202");

    while (true)
    {
        // 앱에서 메시지 받기
        std::string receivedMessage = g_unixBuffer;

        // 데이터 전송
        ssize_t bytes_sent = sendto(udp_socket, receivedMessage.c_str(), receivedMessage.size(), 0, (struct sockaddr *)&server_address, sizeof(server_address));
        if (bytes_sent == -1)
        {
            perror("sendto");
            // return 1;
        }

        std::cout << "Message sent to shared memory Task: " << receivedMessage <<std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_DELAY * 10));
    }
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