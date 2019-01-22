//
// Created by yanpan on 2019/1/15.
//

/************************网络编程************************/

//基于TCP/IP服务器端编程流程
/*
 * 1.创建 socket
 * 2.命名 socket  bind()函数
 * 3.监听 socket  listen()函数
 * 4.接受连接      accept()函数
 * 5.接收消息      recv()函数
 * 6.发送消息      send()函数
 * 7.关闭连接      close()函数
 */

//七大步骤的细节
/*
 * 1.创建 socket   int socket(int domain, int type, int protocol)
 *   成功时返回一个socket  失败时返回-1并设置errno;
 *   参数demain: 选择使用哪个底层协议族 PF_INET(ipv4) PF_INET6(ipv6)
 *   第二个参数 type:选择服务类型 SOCK_STREAM(流服务)   SOCK_UGRAM(数据报服务)
 *   第三个参数 protocol: 选择前边两个协议后，这个参数还是选择协议的参数，默认值为0，表示使用默认协议
 *
 * 2.命名 socket   int bind(int sockfd, const struct sockaddr* my_addr, socklen_t addrlen)
 *   bind函数将 未命名的sockfd文件描述符与my_addr所指的socket绑定, addrlen参数是指该socket地址的长度
 *   bind函数成功时返回0，失败时返回-1 并设置errno -> 两个常见的error 如下：
 *   EACCES: 被绑定的地址是受保护的地址，仅超级用户才能够访问
 *   EADDRINUSE：被绑定的地址正在使用中。 例如：将sockfd绑定到一个处于TIME_WAIT状态下的socket地址
 *
 * 3.监听 socket   int listen(int sockfd, int backlog)
 *   参数sockfd是指被监听的socket， 参数backlog是指内核监听队列的最大长度
 *   监听队列的长度如果超过backlog，服务器将不受理新的客户端连接，客户端会收到ECONNREFUSED的错误信息
 *   linux内核2.2版本之后表示处于ESTABLISHED(完全连接状态)的socket的上限，backlog参数的典型值是5
 *   listen函数成功时返回0，失败则返回-1 并设置errno
 *
 * 4.接受连接       int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
 *   sockfd指的是经过listen系统调用的监听socket  addr参数用来获取被接受连接的远端socket地址,addrlen为其长度
 *   accept成功时返回一个新的连接socket，该socket唯一标识了这个被接受的连接，
 *   服务器可通过读写该socket来与被接受连接对应的客户端进行通信。
 *   accept失败时返回-1并设置error
 *
 * 5.数据的读       ssize_t recv(int sockfd, void* buf, siez_t len, int flags)
 *   recv函数读取参数sockfd上的数据   参数buf和len分别是指读缓冲区的位置和大小   参数flags为数据收发提供了额外的控制，通常设置为0
 *   recv函数成功时返回实际读取到的数据的长度,它可能小于len,因此可能会多次调用recv。
 *   当recv返回0，意味着通信对方已经关闭了连接
 *   失败则返回-1，并设置errno
 *
 * 6.数据的写       ssize_t send(int sockfd, const void* buf, size_t len, int flags)
 *   send函数往参数sockfd写入数据   参数buf和len分别是指写缓冲区的位置和大小   参数flags为数据收发提供了额外的控制，通常设置为0
 *   send成功返回实际写入数据的长度，失败返回-1并设置errno
 *
 * 7.关闭连接       int close(int fd)
 *   参数fd是待关闭的socket
 *   close系统调用并不是立即关闭 而是将fd的引用计数为0时才真正关闭连接
 *   多进程程序中，一次fork系统调用默认父进程打开得socket的引用计数加1，所以要将父子进程中的socket都关闭才是真正断开连接
 *
 *   如果想立即终止连接 可以使用下列的函数 
 *   int shutdown(int fd,int howto)
 *   howto决定了关闭的行为
 *   SHUT_RD 关闭socket读的一端,应用程序不能针对socket进行读操作,并且该socket接受缓冲区中的数据都被丢弃
 *   SHUT_WR 关闭socket写的一端,在关闭之前将socket发送缓冲区的数据全部发送出去,应用程序不能针对socket进行写操作,该情况下,经常处于半关闭状态
 *   SHUT_RDWR 同时关闭socket上的读与写
 *   shutdown函数成功时返回0, 失败则返回-1,并设置errno
 */

//基于TCP/IP客户端编程流程
/*
 * 1.创建 socket
 * 2.发起连接      connect()函数
 * 3.接收消息      recv()函数
 * 4.发送消息      send()函数
 * 5.关闭连接      close()函数
 */

//客户端编程的函数细节，1、2、4、5、6与服务器端相同
/*
 * 2.发起连接      int connect(int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen)
 *   参数sockfd 是由系统调用返回的一个socket   参数serv_addr 是服务器监听的socket地址,参数addrlen指这个地址的长度
 *   成功时返回0 一旦连接成功 sockfd就唯一标识了这个连接,客户端可通过读写该sockfd来与服务器进行通信。
 *   失败时返回-1并设置errno  ->  ECONNREFUSED 目标端口不存在 连接被拒绝
 *                             ETIMEDOUT 连接超时
 */

#if 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/select.h>


void main()
{
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);  //创建套接字
    if(-1 == sockfd)
        return;

    struct sockaddr_in ser,cli;     //在绑定函数(bind();)中需要的结构体，用来记录客户端的ip地址和端口号
    ser.sin_family = PF_INET;       //地址族：Tcp/Ip
    ser.sin_port = htons(6000);     //将客户端的端口号转化为网络字节序
    ser.sin_addr.s_addr = inet_addr("127.0.0.1"); //将客户端的ip地址转化为网络字节序  "127.0.0.1"是连接本机的ip地址

    int ret = bind(sockfd, (struct sockaddr*)&ser, sizeof(ser));   //命名套接字
    if(-1 == ret)
        return;

    int listen_fd = listen(sockfd, 5);
    if(-1 == listen_fd)
        return;
    int len = sizeof(cli);
    //连接套接字  通过c实现服务器与客户端之间的通信
    int c = accept(sockfd, (struct sockaddr*)&cli, &len);
    if(-1 == c)
				return;

    while(1)
    {
        char buff[256] = {0};
        //接收客户端消息
        int n = recv(c, buff, 128, 0);
        if(n <= 0)
        {
            //关闭连接
            close(c);
            return;
        }
        printf("buff: %s\n", buff);
        //向客户端发送消息  "👌"，接收成功
        send(c, "ok", 2, 0);
    }
		close(sockfd);
}

#endif

