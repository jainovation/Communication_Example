#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    const int server_port = 34567;  // 서버 포트 번호

    // 소켓 생성
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Failed to create socket");
        return 1;
    }

    sockaddr_in server_addr, client_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // 소켓 바인딩
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed");
        return 1;
    }

    std::cout << "UDP 서버 시작. 포트: " << server_port << std::endl;

    char buffer[1024];
    socklen_t client_addr_len = sizeof(client_addr);

    while (true) {
        memset(buffer, 0, sizeof(buffer));

        // 클라이언트로부터 데이터 수신
        int bytes_received = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &client_addr_len);
        if (bytes_received < 0) {
            perror("Error receiving data");
            return 1;
        }

        std::cout << "클라이언트로부터 수신한 메시지: " << buffer << std::endl;

        // 클라이언트에게 응답
        const char* response = "서버에서 응답하는 메시지!";
        sendto(sockfd, response, strlen(response), 0, (struct sockaddr*)&client_addr, client_addr_len);
    }

    close(sockfd);
    return 0;
}







