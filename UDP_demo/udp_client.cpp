#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <fstream>
#include <csignal>
#include <iostream>

#define PORT 12348
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
    int so_broadcast;
    fd_set readfd;
    int recv_len;
    socklen_t from_len = sizeof(struct sockaddr_in);
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;
    struct timeval timeout;
    timeout.tv_sec = 2; //超时时间为2秒
    timeout.tv_usec = 0;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("create socket failed:");
        return -1;
    }
    //默认的套接字描述符sock是不支持广播，必须设置套接字描述符以支持广播
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof(so_broadcast));

    //broadcast_addr.sin_addr.s_addr = htons(INADDR_ANY);  //本机ip地址连接
    client_addr.sin_addr.s_addr = inet_addr("192.168.1.113"); //指定IP连接
    //设置广播端口号
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(PORT);
    //发送多次广播，看网络上是否有服务器存在
    char sendbuf[1024];
    while (!stopFlag)
    {
        char send_msg[1024] = "this is a demo";
        memcpy(sendbuf, send_msg, sizeof(send_msg));
        ret = sendto(sock, sendbuf, strlen(sendbuf), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
        if (ret == -1)
        {
            std::cout << "send msg error" << std::endl;
            break;
        }
        //select侦听是否有数据到来
        memset(sendbuf, 0, sizeof(sendbuf));
        memset(send_msg, 0, sizeof(send_msg));
        sleep(10);
    }
    close(sock);
    return 0;
}