#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/select.h>
#include <assert.h>
#define MAXFD 10

//初始化数组
void fds_init(int *arr)
{
    int i = 0;
    for(; i < MAXFD; i++)
    {
        arr[i] = -1;
    }
}

void fds_add(int *arr, int fd)
{
    int i = 0;
    for(; i < MAXFD; i++)
    {
        if(arr[i] == -1)
        {
            arr[i] = fd;
            break;
        }
    }
    
}
//删除指定的描述符
void fds_del(int *arr, int fd)
{
    int i = 0;
    for(; i < MAXFD; i++)
    {
        if(arr[i] == fd)
        {
            arr[i] = -1;
            break;
        }
    }
}

int creat_sock()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(sockfd != -1);
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8000);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    int ret = bind(sockfd, (struct sockaddr*)&saddr, sizeof(saddr));
    assert(ret != -1);

    listen(sockfd, 5);

    return sockfd;
}

int main()
{   
    struct sockaddr_in caddr;
    int sockfd = creat_sock();

    int fds[MAXFD];
    fds_init(fds);

    fds_add(fds, sockfd);
    while(1)
    {
        fd_set fdset;
        FD_ZERO(&fdset);
        int maxfd = -1;
        int i = 0;
        for(; i < MAXFD; i++)
        {
            if(fds[i] == -1)
                continue;

            FD_SET(fds[i], &fdset);
            if(fds[i] > maxfd)
            {
                maxfd = fds[i];
            }
        }

        struct timeval tv = {5, 0};
        
        int n = select(maxfd+1, &fdset, NULL, NULL, &tv);
        if(n == -1)
        {
            printf("select error\n");
            continue;
        }
        else if(n == 0)
        {
            printf("select timeout\n");
            continue;
        }
        else
        {
            int i = 0;
            for(; i < MAXFD; i++)
            {
                if(fds[i] == -1)
                    continue;
                if(FD_ISSET(fds[i], &fdset))
                {
                    if(fds[i] == sockfd)
                    {
                        socklen_t len = sizeof(caddr);
                        int c = accept(sockfd, (struct sockaddr*)&caddr, &len);
                        if(c < 0)
                            continue;
                        printf("accept c = %d success\n", c);
                        fds_add(fds, c);
                    }
                    else
                    {
                        char buff[128] = {0};
                        int num = recv(fds[i], buff, 127, 0);
                        if(num <= 0)
                        {
                            close(fds[i]);
                            fds_del(fds, fds[i]);
                            printf("one client close\n");
                        }
                        else
                        {
                            printf("recv(%d) = %s\n", fds[i], buff);
                            send(fds[i], "ok", 2, 0);
                        }
                    }
                }
            }
        }
    }

    return 0;
}
