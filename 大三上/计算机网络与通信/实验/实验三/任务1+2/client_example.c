#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // 检查命令行参数数量，确保用户输入了服务器的IP地址和端口号
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <port>\n", argv[0]);
        // 如果命令行参数不正确，打印用法信息并退出程序
        exit(1);
    }

    int client_sock;
    struct sockaddr_in server_addr;
    char send_msg[255]; // 要发送的消息
    char recv_msg[255]; // 用于存储接收到的消息的数组

    char IP[17];
    if(strcmp(argv[1], "localhost") == 0){
        strcpy(IP, "127.0.0.1");
    } else {
        strcpy(IP, argv[1]);
    }

    /* 创建socket */
    if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        // socket函数创建套接字，如果失败，打印错误信息并退出程序
        exit(1);
    }

    /* 指定服务器地址 */
    server_addr.sin_family = AF_INET; // 使用IPv4地址
    server_addr.sin_port = htons(atoi(argv[2])); // 将命令行参数中的端口号转换为网络字节序
    inet_aton(IP, &server_addr.sin_addr); // 将字符串形式的IP地址转换为网络字节序

    /* 连接服务器 */
    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        // 连接服务器，如果失败，打印错误信息并退出程序
        close(client_sock);
        exit(1);
    }

    while(1) {
        /* 发送消息 */
        printf("Myself: ");
        scanf("%s", send_msg);
        send(client_sock, send_msg, strlen(send_msg), 0); // 发送消息到服务器

        /* 接收并显示消息 */
        memset(recv_msg, 0, sizeof(recv_msg)); // 将接收消息的数组置零
        if (recv(client_sock, recv_msg, sizeof(recv_msg), 0) == -1) {
            perror("recv");
            // 接收消息，如果失败，打印错误信息并退出循环
            break;
        }
        printf("Server: %s", recv_msg); // 打印接收到的消息
        printf("\n");

        /* 如果用户输入"bye"则退出循环 */
        if (strcmp(send_msg, "bye") == 0) {
            break;
        }
    }

    /* 关闭socket */
    close(client_sock); // 关闭客户端套接字

    return 0;
}
