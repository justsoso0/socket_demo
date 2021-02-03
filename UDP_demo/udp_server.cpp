
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <csignal>
#include <iostream>

#define PORT 12345

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

int main(int argc, char *argv[])
{
    signal(SIGINT, stop);
    signal(SIGTERM, stop);
    int ret;
    int sock;
    struct sockaddr_in server_addr; //服务器端地址
    struct sockaddr_in client_addr; //客户端地址
    socklen_t from_len = sizeof(struct sockaddr_in);
    int recv_len;
    sock = socket(AF_INET, SOCK_DGRAM, 0); //建立数据报套接字（UDP参数为SOCK_DGRAM）
    if (sock < 0)
    {
        perror("sock error");
        return -1;
    }
    memset((void *)&server_addr, 0, sizeof(struct sockaddr_in));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(PORT);
    //将地址结构绑定到套接字上
    ret = bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0)
    {
        perror("bind error");
        return -1;
    }

    fd_set readfd, tempfd; //读文件描述符集合
    //文件描述符集合清0
    FD_ZERO(&readfd);
    //将套接字描述符加入到文件描述符集合
    FD_SET(sock, &readfd);
    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
    char buffer[1024];
    /**
	 * 循环等待客户端
	 */
    while (!stopFlag)
    {
        timeout.tv_sec = 0;
        timeout.tv_usec = 500000;
        tempfd = readfd;
        //select侦听是否有数据到来
        ret = select(sock + 1, &tempfd, NULL, NULL, &timeout); //侦听是否可读
        if (ret == -1)
        {
            std::cout << "select error" << std::endl;
            break;
        }
        else if (ret == 0)
        {
            usleep(100000);
            continue;
        }
        if (FD_ISSET(sock, &readfd))
        {
            recv_len = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &from_len); //接收客户端发送的数据
            std::string client_ip = inet_ntoa(client_addr.sin_addr);
            int port = client_addr.sin_port;
            std::cout << "recv client{ip:" << client_ip << ",port:" << port << "} msg:" << buffer << std::endl;
        }
        memset(buffer, 0, sizeof(buffer));
    }
    close(sock);
    return 0;
}