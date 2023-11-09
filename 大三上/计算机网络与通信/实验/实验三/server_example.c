#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "error.h"
#include "stdlib.h"


#define MAX_STUDENTS 10
#define MAX_LINE_LENGTH 256 // 假设每行最多256个字符

void readClassTable(char students[MAX_STUDENTS][MAX_LINE_LENGTH]) {
    FILE *file = fopen("../ClassTable.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    int index = 0;
    while (fgets(students[index], MAX_LINE_LENGTH, file) != NULL && index < MAX_STUDENTS) {
        // 移除末尾的换行符
        students[index][strcspn(students[index], "\n")] = '\0';
        index++;
    }

    fclose(file);
}

int cmp(char *a,char *b,int n){
    for (int i = 0; i < n; ++i) {
        if(a[i]!=b[i])return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        // 如果命令行参数不正确，打印用法信息并退出程序
        exit(1);
    }

    char students[MAX_STUDENTS][MAX_LINE_LENGTH]; // 二维字符数组，每行存储一个学生的信息
    readClassTable(students);

	int server_sock_listen, server_sock_data;
	struct sockaddr_in server_addr;
	char recv_msg[255];


    /* 创建socket */
	server_sock_listen = socket(AF_INET, SOCK_STREAM, 0);


    /* 指定服务器地址 */
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[1]));
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //INADDR_ANY表示本机所有IP地址
	memset(&server_addr.sin_zero, 0, sizeof(server_addr.sin_zero)); //零填充

	/* 绑定socket与地址 */
	bind(server_sock_listen, (struct sockaddr *)&server_addr, sizeof(server_addr));

	/* 监听socket */
	listen(server_sock_listen, 0);

	server_sock_data = accept(server_sock_listen, NULL, NULL);

	/* 接收并显示消息 */

    while(1) {
        memset(recv_msg, 0, sizeof(recv_msg)); //接收数组置零
        recv(server_sock_data, recv_msg, sizeof(recv_msg), 0);
        printf("client:%s\n",recv_msg);
        if (strcmp(recv_msg, "bye") == 0) {
            strcpy(recv_msg,"bye");
            break;
        }
        int i=0;
        while(i<MAX_STUDENTS){
            if(cmp(students[i],recv_msg,3)==0)break;
            i++;
        }
        if(i>=MAX_STUDENTS)strcpy(recv_msg,"not exit!");
        else if(recv_msg[3]==0 || recv_msg[5]=='0')strcpy(recv_msg,students[i]);
        else{
            char class[10];
            memset(class,0,sizeof class);
            strcpy(class,recv_msg+4);
            int n=0;
            for (int j = 0; j< strlen(class);++j) {
                n+= class[j]-'0';
                n*=10;
            }
            n/=10;
            int cnt=-1;
            int j=0;
            for (; j < strlen(students[i]); ++j) {
                if(students[i][j]==' ')cnt++;
                if(cnt==n)break;
            }
            if(cnt!=n)strcpy(recv_msg,"not exit!");
            else{
                int m=0;
                cnt=0;
                int k=0;
                while(1){
                    if(students[i][k]==' ')cnt++;
                    if(cnt>=2)break;
                    recv_msg[m]=students[i][k];
                    k++;
                    m++;
                }
                k=j;
                cnt=-1;
                while(k< strlen(students[i])){
                    if(students[i][k]==' ')cnt++;
                    if(cnt==1)break;
                    recv_msg[m]=students[i][k];
                    k++;
                    m++;
                }
            }

        }
        /* 发送消息 */
        printf("Send: %s\n", recv_msg);

        send(server_sock_data, recv_msg, strlen(recv_msg), 0);
    }

	/* 关闭数据socket */
	close(server_sock_data);

	/* 关闭监听socket */
	close(server_sock_listen);

	return 0;
}
