#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>
#include <strings.h>

#define PORT 8888               //服务器端监听端口号,这个端口号可以自行定义
#define MAX_BUFFER 1024         //数据缓冲区最大值

#define TAG "Service"
int main()
{
    struct sockaddr_in server_addr, client_addr;
    int server_sockfd, client_sockfd;
    int size, write_size;
    char buffer[MAX_BUFFER];

    if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)    //创建Socket
    {
        perror("Socket Created Failed!\n");
        exit(1);
    }
    printf("Socket Create Success!\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);
    bzero(&(server_addr.sin_zero), 8);

    int opt = 1;
    int res = setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));    //设置地址复用
    if (res < 0)
    {
        perror("Server reuse address failed!\n");
        exit(1);
    }

    if (bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)  //绑定本地地址
    {
        perror("Socket Bind Failed!\n");
        exit(1);
    }
    printf("Socket Bind Success!\n");

    if (listen(server_sockfd, 5) == -1)                 //监听
    {
        perror("Listened Failed!\n");
        exit(1);
    }
    printf("Listening ....\n");

    socklen_t len = sizeof(client_addr);

    printf("waiting connection...\n");
    if ((client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &len)) == -1)  //等待客户端连接
    {
        perror("Accepted Failed!\n");
        exit(1);
    }

    printf("connection established!\n");
    printf("waiting message...\n");
    while (1)
    {
        memset(buffer, 0, sizeof(buffer));                             //清空数据缓冲区

        if ((size = read(client_sockfd, buffer, MAX_BUFFER)) == -1)    //读取客户端的数据
        {
            perror("Recv Failed!\n");
            exit(1);
        }

        if (size != 0)                                               
        {
            buffer[size] = '\0';
            printf("Recv msg from client: %s\n", buffer);
            if ((write_size = write(client_sockfd, buffer, MAX_BUFFER)) > 0)   //把收到的数据回发给客户端
            {
                printf("Sent msg to client successfully!\n");
            }

        }
    }

    close(client_sockfd);   //关闭Socket
    close(server_sockfd);

    return 0;
}


