//
// Created by yanpan on 2019/1/21.
//


#if 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

void main()
{
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);  //创建套接字
    if(-1 == sockfd)
        return;

    struct sockaddr_in ser,cli;     //在绑定函数(bind();)中需要的结构体，用来记录客户端的ip地址和端口号
    ser.sin_family = PF_INET;       //地址族：Tcp/Ip
    ser.sin_port = htons(6000);     //将客户端的端口号转化为网络字节序
    ser.sin_addr.s_addr = inet_addr("127.0.0.1"); //将客户端的ip地址转化为网络字节序  "127.0.0.1"是连接本机的ip地址

    int ret = connect(sockfd, (struct sockaddr*)&ser, sizeof(ser));   //命名套接字
    if(-1 == ret)
        return;
    while(1)
    {
        char buff[128] = {0};
        printf("input:\n");
        fgets(buff, 128, stdin);
        if(strncmp(buff, "end", 3) == 0)
            break;
        send(sockfd, buff, strlen(buff) - 1, 0);
        memset(buff, 0, 128);
        recv(sockfd, buff, 127, 0);
        printf("buff: %s\n", buff);
    }
    close(sockfd);
}

#endif
