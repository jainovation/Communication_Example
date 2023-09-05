#include <syslog.h>

#define TIME_DELAY 50
#define PACKET_SIZE 4096
#define DATA_SIZE 4000
#define MAX_MSG_SIZE 8192
#define SHARED_MEM_SIZE 4096
// #define SHARED_MEM_SIZE 8192
#define QUEUE_NAME "/my_message_queue"
#define SOCKET_PATH "/tmp/my_unix_socket"

// 메시지 구조체 정의
typedef struct _Message
{
    char header[8];
    char data[DATA_SIZE];
    char tail[2];
} __attribute__((packed)) Message_t;