#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    const char* server_ip = "127.0.0.1";  // 서버 IP 주소
    const int server_port = 34567;       // 서버 포트 번호

    // 소켓 생성
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Failed to create socket");
        return 1;
    }

    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    char buffer[1024];
    socklen_t server_addr_len = sizeof(server_addr);

    const char* message = "안녕, 서버!";

    // 서버에 데이터 전송
    if (sendto(sockfd, message, strlen(message), 0, (struct sockaddr*)&server_addr, server_addr_len) < 0) {
        perror("Error sending data");
        return 1;
    }

    // 서버로부터 응답 수신
    memset(buffer, 0, sizeof(buffer));
    int bytes_received = recvfrom(sockfd, buffer, sizeof(buffer), 0, nullptr, nullptr);
    if (bytes_received < 0) {
        perror("Error receiving data");
        return 1;
    }

    std::cout << "서버로부터 수신한 응답: " << buffer << std::endl;

    close(sockfd);
    return 0;
}