#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define MAX_STUDENTS 10
#define MAX_LINE_LENGTH 256

void readClassTable(char students[MAX_STUDENTS][MAX_LINE_LENGTH]) {
    FILE *file = fopen("../ClassTable.txt", "r");
    if (file == NULL) {
        perror("打开文件错误"); // 打开文件错误
        exit(EXIT_FAILURE);
    }

    int index = 0;
    while (fgets(students[index], MAX_LINE_LENGTH, file) != NULL && index < MAX_STUDENTS) {
        students[index][strcspn(students[index], "\n")] = '\0'; // 移除末尾的换行符
        index++;
    }

    fclose(file);
}

int cmp(char *a, char *b, int n) {
    // 比较函数：比较前n个字符是否相同
    for (int i = 0; i < n; ++i) {
        if (a[i] != b[i]) return 1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "用法: %s <端口>\n", argv[0]);
        // 如果命令行参数不正确，打印用法信息并退出程序
        exit(1);
    }

    char students[MAX_STUDENTS][MAX_LINE_LENGTH];
    readClassTable(students);

    int server_sock_listen, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char recv_msg[255];

    /* 创建socket */
    server_sock_listen = socket(AF_INET, SOCK_STREAM, 0);

    /* 指定服务器地址 */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1])); // 将端口转换为网络字节顺序
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY表示本机所有IP地址
    memset(&server_addr.sin_zero, 0, sizeof(server_addr.sin_zero)); // 零填充

    /* 绑定socket与地址 */
    bind(server_sock_listen, (struct sockaddr *)&server_addr, sizeof(server_addr));

    /* 监听socket */
    listen(server_sock_listen, 5); // 允许最多同时连接5个客户端

    while (1) {
        /* 接受客户端连接 */
        client_sock = accept(server_sock_listen, (struct sockaddr *)&client_addr, &client_len);

        if (fork() == 0) { // 子进程
            close(server_sock_listen); // 在子进程中关闭监听socket

            while (1) {
                memset(recv_msg, 0, sizeof(recv_msg));
                recv(client_sock, recv_msg, sizeof(recv_msg), 0);
                printf("Accept %s\n", inet_ntoa(client_addr.sin_addr));
                printf("From %s: %s\n", inet_ntoa(client_addr.sin_addr), recv_msg); // 打印客户端发送的消息

                if (strcmp(recv_msg, "bye") == 0) {
                    printf("Close %s\n", inet_ntoa(client_addr.sin_addr)); // 客户端发送了“bye”消息，结束通信
                    break;
                }

                int i = 0;
                while (i < MAX_STUDENTS) {
                    if (cmp(students[i], recv_msg, 3) == 0) break;
                    i++;
                }
                if (i >= MAX_STUDENTS) strcpy(recv_msg, "not exit！");
                else if (recv_msg[3] == 0 || recv_msg[5] == '0') strcpy(recv_msg, students[i]);
                else {
                    char class[10];
                    memset(class, 0, sizeof class);
                    strcpy(class, recv_msg + 4);
                    int n = 0;
                    for (int j = 0; j < strlen(class); ++j) {
                        n += class[j] - '0';
                        n *= 10;
                    }
                    n /= 10;
                    int cnt = -1;
                    int j = 0;
                    for (; j < strlen(students[i]); ++j) {
                        if (students[i][j] == ' ') cnt++;
                        if (cnt == n) break;
                    }
                    if (cnt != n) strcpy(recv_msg, "not exit！");
                    else {
                        int m = 0;
                        cnt = 0;
                        int k = 0;
                        while (1) {
                            if (students[i][k] == ' ') cnt++;
                            if (cnt >= 2) break;
                            recv_msg[m] = students[i][k];
                            k++;
                            m++;
                        }
                        k = j;
                        cnt = -1;
                        while (k < strlen(students[i])) {
                            if (students[i][k] == ' ') cnt++;
                            if (cnt == 1) break;
                            recv_msg[m] = students[i][k];
                            k++;
                            m++;
                        }
                    }
                }

                /* 发送消息 */
                printf("To %s: %s\n", inet_ntoa(client_addr.sin_addr), recv_msg); // 打印要发送的消息
                send(client_sock, recv_msg, strlen(recv_msg), 0);
            }


            /* 关闭客户端socket */
            close(client_sock);
            exit(0); // 子进程结束
        } else {
            close(client_sock); // 父进程关闭客户端socket
        }
    }

    /* 关闭监听socket */
    close(server_sock_listen);

    return 0;
}