#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <map>
#include <csignal>

int epollfd_ = 0;
int listenfd_;
std::map<int, std::string> get_sockt;
#define PORT 1502
#define MAXEVENTSSIZE 20
#define TIMEWAIT 100
#define MAXBUFFSIZE 2048

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
int set_noblock(int fd)
{
    int flags;
    if ((flags = fcntl(fd, F_GETFL, 0)) == -1)
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void ctl_event(int fd, bool flag)
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = flag ? EPOLLIN : 0;
    printf("fd: %d begin to ctl epoll\n", fd);
    epoll_ctl(epollfd_, flag ? EPOLL_CTL_ADD : EPOLL_CTL_DEL, fd, &ev);
    printf("fd: %d end to ctl epoll\n", fd);
    if (flag)
    {
        set_noblock(fd);
        // if (fd != listenfd_)
        printf("fd: %d 加入epoll循环\n", fd);
    }
    else
    {
        close(fd);
        printf("fd: %d 退出epoll循环\n", fd);
    }
}

int init()
{
    int val = 1;
    listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd_ == -1)
    {
        printf("创建套接字失败!\n");
        return -1;
    }
    //一般来说，一个端口释放后会等待两分钟之后才能再被使用，SO_REUSEADDR是让端口释放后立即就可以被再次使用
    int ret = setsockopt(listenfd_, SOL_SOCKET, SO_REUSEADDR, (void *)&val, sizeof(int));
    if (ret == -1)
    {
        printf("setsockopt\n");
        return -1;
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);
    if (-1 == bind(listenfd_, (struct sockaddr *)(&server_addr), sizeof(server_addr)))
    {
        printf("绑定套接字失败!\n");
        return -1;
    }
    if (-1 == listen(listenfd_, 5))
    {
        printf("监听失败!\n");
        return -1;
    }
    epollfd_ = epoll_create(MAXEVENTSSIZE);
    ctl_event(listenfd_, true);
    printf("服务器启动成功!\n");
    return 0;
}

int epoll_loop()
{
    struct sockaddr_in client_addr;
    socklen_t clilen;
    int nfds = 0;
    int fd = 0;
    int bufflen = 0;
    char buff[MAXBUFFSIZE];
    struct epoll_event events[MAXEVENTSSIZE];
    while (!stopFlag)
    {
        printf("begin to epoll wait!\n");
        nfds = epoll_wait(epollfd_, events, MAXEVENTSSIZE, TIMEWAIT);
        if (nfds == 0)
        {
            sleep(1);
            printf("end to epoll wait:%d!\n", nfds);
            continue;
        }
        for (int i = 0; i < nfds; i++)
        {
            if (events[i].data.fd == listenfd_)
            {
                fd = accept(listenfd_, (struct sockaddr *)&client_addr, &clilen);
                printf("accepted a new connetction\n");
                std::string new_ip = inet_ntoa(client_addr.sin_addr);
                int new_port = client_addr.sin_port;
                printf("new socket id is %d\n", fd);
                printf("Accept clent ip is %s;port is %d\n", new_ip.c_str(), new_port);
                std::string new_connect = new_ip + ":" + std::to_string(new_port);
                get_sockt.insert(std::pair<int, std::string>(fd, new_connect));
                ctl_event(fd, true);
            }
            else if (events[i].events & EPOLLIN)
            {
                if ((fd = events[i].data.fd) < 0)
                    continue;
                if (bufflen = read(fd, buff, sizeof(buff)) <= 0)
                {
                    std::cout << "socket reea{" + get_sockt[fd] + "} failed" << std::endl;
                    ctl_event(fd, false);
                    auto itr = get_sockt.find(fd);
                    get_sockt.erase(itr);
                    continue;
                }
                else
                {
                    if (buff == "name")
                    {
                        char buf[1024] = "my name is wxh";
                        send(fd, buf, sizeof(buf), 0);
                    }
                    else if (buff == "age")
                    {
                        char buf[1024] = "18";
                        send(fd, buf, sizeof(buf), 0);
                    }
                    else
                    {
                        char buf[1024] = "it error question";
                        send(fd, buf, sizeof(buf), 0);
                    }
                }
            }
            else
            {
                printf("fd:%d is other fd\n", i);
            }
        }
    }
    return 0;
}

int main()
{
    signal(SIGINT, stop);
    signal(SIGTERM, stop);
    if (init() == 0)
    {
        printf("socket init success\n");
        epoll_loop();
    }
    close(epollfd_);
    for (auto itr = get_sockt.begin(); itr != get_sockt.end(); itr++)
        close(itr->first);
    close(listenfd_);
    return 0;
}
