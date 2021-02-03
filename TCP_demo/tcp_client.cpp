#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <csignal>
#include <iostream>
#include <arpa/inet.h>
#include <thread>

std::string ip = "192.168.1.118";
int port = 1502;
int sockfd;
char sendbuf[1024];
char recvbuf[2014];

bool stopFlag = false;

void stop(int sig)
{
    switch (sig)
    {
    case SIGINT:
        break;
    case SIGTERM:
        break;
    default:
        return;
    }
    stopFlag = true;
}

void sock_recv()
{
    fd_set refset, rdset;
    timeval tv;
    int over_time;
    FD_ZERO(&refset);
    FD_SET(sockfd, &refset);
    while (!stopFlag)
    {
        tv.tv_sec = 0;
        tv.tv_usec = 500000;
        rdset = refset;
        over_time = select(sockfd + 1, &rdset, NULL, NULL, &tv);
        if (over_time == -1)
        {
            std::cout << "socket select error" << std::endl;
            stopFlag = true;
            break;
        }
        else if (over_time == 0)
        {
            usleep(100000);
            continue;
        }
        if (FD_ISSET(sockfd, &refset))
        {
            int len = recv(sockfd, recvbuf, sizeof(recvbuf), 0);
            if (len <= 0)
            {
                std::cout << "socket recv error" << std::endl;
                stopFlag = true;
                break;
            }
            std::cout << "sockt recv msg:" << recvbuf << std::endl;
            memset(recvbuf, 0, sizeof(recvbuf));
        }
    }
}

int main()
{
    signal(SIGINT, stop);
    signal(SIGTERM, stop);

    struct hostent *host;
    int nbytes;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Socket Error is %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    puts("create socket success");
    printf("sockfd is %d\n", sockfd);
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    // server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //客户端发出请求
    if (connect(sockfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr)) == -1)
    {
        fprintf(stderr, "Connect failed\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    int keepalive = 1;    // 开启keepalive属性
    int keepidle = 30;    // 如该连接在60秒内没有任何数据往来,则进行探测
    int keepinterval = 5; // 探测时发包的时间间隔为5 秒
    int keepcount = 1;    // 探测尝试的次数.如果第1次探测包就收到响应了,则后2次的不再发.
    setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive, sizeof(keepalive));
    setsockopt(sockfd, SOL_TCP, TCP_KEEPIDLE, (void *)&keepidle, sizeof(keepidle));
    setsockopt(sockfd, SOL_TCP, TCP_KEEPINTVL, (void *)&keepinterval, sizeof(keepinterval));
    setsockopt(sockfd, SOL_TCP, TCP_KEEPCNT, (void *)&keepcount, sizeof(keepcount));

    std::thread thr(&sock_recv);
    int ret;
    while (!stopFlag)
    {
        fgets(sendbuf, sizeof(sendbuf), stdin);
        ret = send(sockfd, sendbuf, strlen(sendbuf), 0);
        if (ret == -1)
        {
            std::cout << "send msg error" << std::endl;
        }
        memset(sendbuf, 0, sizeof(sendbuf));
        usleep(500000);
    }
    thr.join();
    close(sockfd);
    exit(EXIT_SUCCESS);
    return 0;
}