#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>

void appA() {
    while (true) {
        // App A 작업 수행
        std::cout << "App A is running..." << std::endl;

        int udp_socket;
        struct sockaddr_in server_address, client_address;
        char buffer[1024];

        // 소켓 생성
        if ((udp_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
            perror("socket");
            // return 1;
        }

        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = INADDR_ANY;
        server_address.sin_port = htons(8081); // 포트 번호

        // 소켓 바인딩
        if (bind(udp_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
            perror("bind");
            // return 1;
        }

        socklen_t client_address_len = sizeof(client_address);

        while (true) {
            // 클라이언트로부터 데이터 수신
            ssize_t bytes_received = recvfrom(udp_socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_address, &client_address_len);
            if (bytes_received == -1) {
                perror("recvfrom");
                // continue;
            }

            std::cout << "Received message from client: " << buffer << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

void appB() {
    while (true) {
        // App B 작업 수행
        std::cout << "App B is running..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

int main() {

    std::thread threadA(appA);
    std::thread threadB(appB);

    // 스레드 대기
    threadA.join();
    threadB.join();

    return 0;
}
