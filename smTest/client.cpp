#include <iostream>
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

    // 공유 메모리에 연결
    int shmid = shmget(key, sizeof(SharedData), 0666);
    if (shmid == -1) {
        std::cerr << "공유 메모리 연결 실패" << std::endl;
        return 1;
    }

    // 공유 메모리를 프로세스의 메모리 공간에 연결
    SharedData* sharedData = (SharedData*)shmat(shmid, NULL, 0);

    // 서버로부터 데이터 읽기
    std::cout << "클라이언트: 서버로부터 받은 값: " << sharedData->value << std::endl;
    std::cout << "클라이언트: 서버로부터 받은 메시지: " << sharedData->message << std::endl;

    // 공유 메모리 연결 해제
    shmdt(sharedData);

    return 0;
}