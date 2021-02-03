#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <iostream>
#include <netinet/tcp.h>
#include <csignal>
#include <map>

#define PORT 1502
#define MAXSIZE 1024

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
int main()
{
    signal(SIGINT, stop);
    signal(SIGTERM, stop);
    int sockfd, newsockfd, fdmax;
    //定义服务端套接口数据结构
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int sin_zise, portnumber;
    fd_set refset;
    fd_set rdset;
    timeval tv;
    int overTime = 0;
    //发送数据缓冲区
    char buf[12];
    //定义客户端套接口数据结构
    int addr_len = sizeof(struct sockaddr_in);
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "create socket failed\n");
        exit(EXIT_FAILURE);
    }
    puts("create socket success");
    printf("sockfd is %d\n", sockfd);
    //清空表示地址的结构体变量
    bzero(&server_addr, sizeof(struct sockaddr_in));
    //设置addr的成员变量信息
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    //设置ip为本机IP
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sockfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr)) < 0)
    {
        fprintf(stderr, "bind failed \n");
        exit(EXIT_FAILURE);
    }
    puts("bind success\n");
    if (listen(sockfd, 10) < 0)
    {
        perror("listen fail\n");
        exit(EXIT_FAILURE);
    }
    puts("listen success\n");
    socklen_t sin_size = sizeof(struct sockaddr_in);
    printf("sin_size is %d\n", sin_size);
    memset(&client_addr, 0, sizeof(client_addr));
    FD_ZERO(&refset);
    FD_SET(sockfd, &refset);
    fdmax = sockfd;
    std::map<int, std::string> get_sockt;
    int master_socket;
    char sendbuf[1024];
    char recvbuf[1024];
    // int keepalive = 1;    // 开启keepalive属性
    // int keepidle = 30;    // 如该连接在60秒内没有任何数据往来,则进行探测
    // int keepinterval = 5; // 探测时发包的时间间隔为5 秒
    // int keepcount = 3;    // 探测尝试的次数.如果第1次探测包就收到响应了,则后2次的不再发.
    // setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive, sizeof(keepalive));
    // setsockopt(sockfd, SOL_TCP, TCP_KEEPIDLE, (void *)&keepidle, sizeof(keepidle));
    // setsockopt(sockfd, SOL_TCP, TCP_KEEPINTVL, (void *)&keepinterval, sizeof(keepinterval));
    // setsockopt(sockfd, SOL_TCP, TCP_KEEPCNT, (void *)&keepcount, sizeof(keepcount));
    while (!stopFlag)
    {
        tv.tv_sec = 0;
        tv.tv_usec = 500000;
        rdset = refset;
        overTime = select(fdmax + 1, &rdset, NULL, NULL, &tv);
        if (overTime == -1)
        {
            std::cout << "select error" << std::endl;
            stopFlag = true;
            break;
        }
        else if (overTime == 0)
        {
            usleep(100000);
            continue;
        }
        for (master_socket = 0; master_socket <= fdmax; master_socket++)
        {
            if (!FD_ISSET(master_socket, &rdset))
            {
                continue;
            }
            if (master_socket == sockfd)
            {
                socklen_t addrlen;
                struct sockaddr_in clientaddr;
                int newfd;
                addrlen = sizeof(clientaddr);
                memset(&clientaddr, 0, sizeof(clientaddr));
                newfd = accept(sockfd, (struct sockaddr *)&clientaddr, &addrlen);
                if (newfd == -1)
                {
                    std::cout << "server accept failed" << std::endl;
                }
                else
                {
                    std::cout << "get newfd:" << newfd << std::endl;
                    FD_SET(newfd, &refset);
                    if (newfd > fdmax)
                    {
                        /* Keep track of the maximum */
                        fdmax = newfd;
                    }
                    printf("accepted a new connetction\n");
                    std::string new_ip = inet_ntoa(clientaddr.sin_addr);
                    int new_port = clientaddr.sin_port;
                    printf("new socket id is %d\n", newfd);
                    printf("Accept clent ip is %s;port is %d\n", new_ip.c_str(), new_port);
                    std::string new_connect = new_ip + ":" + std::to_string(new_port);
                    get_sockt.insert(std::pair<int, std::string>(newfd, new_connect));
                }
            }
            else
            {
                int len = recv(master_socket, recvbuf, sizeof(recvbuf), 0);
                if (len <= 0)
                {
                    std::cout << "socket recv{" + get_sockt[master_socket] + "} failed" << std::endl;
                    close(master_socket);
                    FD_CLR(master_socket, &refset);
                    if (master_socket == fdmax)
                        fdmax--;
                    auto itr = get_sockt.find(master_socket);
                    get_sockt.erase(itr);
                    continue;
                }
                if (recvbuf == "name")
                {
                    char buf[1024] = "my name is wxh";
                    send(master_socket, buf, sizeof(buf), 0);
                }
                else if (recvbuf == "age")
                {
                    char buf[1024] = "18";
                    send(master_socket, buf, sizeof(buf), 0);
                }
                else
                {
                    char buf[1024] = "it error question";
                    send(master_socket, buf, sizeof(buf), 0);
                }
            }
        }
    };
    for (auto itr = get_sockt.begin(); itr != get_sockt.end(); itr++)
        close(itr->first);
    close(sockfd);
    puts("exit success");
    exit(EXIT_SUCCESS);
    return 0;
}
