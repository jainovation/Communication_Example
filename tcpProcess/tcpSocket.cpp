#include <iostream>
#include <mqueue.h>
#include <thread>
#include <chrono>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define QUEUE_NAME "/my_message_queue"
#define MAX_MSG_SIZE 1024

char g_mqBuffer[MAX_MSG_SIZE];

void appA()
{
    // MQ 버퍼
    mqd_t mq;
    int cnt = 0;
    // char mqBuffer[MAX_MSG_SIZE];

    while (true)
    {
        // 메시지 큐 열기
        mq = mq_open(QUEUE_NAME, O_RDONLY);
        if (mq == -1)
        {
            perror("mq_open");
            // return 1;
        }

        // 메시지 받기
        ssize_t bytes_read = mq_receive(mq, g_mqBuffer, MAX_MSG_SIZE, nullptr);
        if (bytes_read == -1)
        {
            perror("mq_receive");
            // return 1;
        }

        std::cout << "Received message from queue: " << g_mqBuffer << std::endl;
        std::cout << "cnt: " << cnt++ << std::endl;

        mq_close(mq);

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

void appB()
{
    // TCP 버퍼
    int client_fd;
    struct sockaddr_in server_address;
    int cnt = 0;
    // char tcpBuffer[1024];

    /* TCP */
    // 소켓 생성
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        // return 1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);

    // IP 주소 설정
    if (inet_pton(AF_INET, "172.24.158.228", &server_address.sin_addr) <= 0)
    {
        perror("Invalid address/ Address not supported");
        // return 1;
    }

    // 서버에 연결
    if (connect(client_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Connection failed");
        // return 1;
    }
    /* _TCP */

    while (true)
    {
        // 앱에서 메시지 받기
        std::string receivedMessage = g_mqBuffer;

        // 메시지 전송
        send(client_fd, receivedMessage.c_str(), receivedMessage.size(), 0);
        std::cout << "Message sent to unixDomainTask app: " << receivedMessage << std::endl;

        // close(client_fd);

        std::this_thread::sleep_for(std::chrono::seconds(5));
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