#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// 공유 메모리에서 사용할 구조체 정의
struct SharedData {
    int value;
    char message[256];
};

int main() {
    // 공유 메모리 키 생성
    key_t key = ftok("shared_memory_example", 1);

    // 공유 메모리 생성 또는 연결
    int shmid = shmget(key, sizeof(SharedData), 0666 | IPC_CREAT);
    if (shmid == -1) {
        std::cerr << "공유 메모리 생성 또는 연결 실패" << std::endl;
        return 1;
    }

    // 공유 메모리를 프로세스의 메모리 공간에 연결
    SharedData* sharedData = (SharedData*)shmat(shmid, NULL, 0);

    // 데이터를 공유 메모리에 쓰기
    sharedData->value = 42;
    strncpy(sharedData->message, "Hello, Client!", sizeof(sharedData->message));

    // 클라이언트로부터 메시지 수신 대기
    std::cout << "서버: 클라이언트로부터 메시지 수신 대기 중..." << std::endl;
    while (true) {
        sleep(1);
    }

    // 공유 메모리 연결 해제
    shmdt(sharedData);

    // 공유 메모리 삭제
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}