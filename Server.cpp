#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include "utils.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void setnonblocking(int fd){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int main(){
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    errif(sockfd == -1, "socket create error");

    struct sockaddr_in serv_addr;//结构体sockaddrin包含在inet.h中，包含了地址族端口号与ip地址

    bzero(&serv_addr,sizeof(serv_addr));//初始化

    serv_addr.sin_family = AF_INET;//协议族ip4
    serv_addr.sin_addr.s_addr= inet_addr("127.0.0.1");//IP地址，这一串没太看懂，先用着
    serv_addr.sin_port = htons(8888);

    errif(bind(sockfd,(sockaddr*)&serv_addr,sizeof(serv_addr)) == -1,"socket bind error");//bind函数，挺有意思的

    errif(listen(sockfd, SOMAXCONN) == -1, "socket listen error");//监听sockfd端口

    int epfd = epoll_create1(0);
    errif(epfd == -1, "epoll create error");

    struct epoll_event events[MAX_EVENTS], ev;
    bzero(&events,sizeof(events));
    ev.events =EPOLLIN;
    ev.data.fd= sockfd;
    setnonblocking(sockfd);
    epoll_ctl(epfd,EPOLL_CTL_ADD,sockfd,&ev);

    while(1){
        int nfds = epoll_wait(epfd, events ,MAX_EVENTS, -1);
        for(int i=0;i<nfds;++i){
            if(events[i].data.fd==sockfd){
                struct sockaddr_in clnt_addr;
                bzero(&clnt_addr,sizeof(clnt_addr));
                socklen_t clnt_addr_len = sizeof(clnt_addr);

                int clnt_sockfd = accept(sockfd,(sockaddr*)&clnt_addr,&clnt_addr_len);
                errif(clnt_sockfd == -1,"socket accept error");
                printf("new client fd %d! IP: %s Port: %d\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
                
                bzero(&ev,sizeof(ev));
                ev.data.fd = clnt_sockfd;
                ev.events = EPOLLIN | EPOLLET;
                setnonblocking(clnt_sockfd);
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sockfd, &ev);
            }else if(events[i].events & EPOLLIN){
                char buf[READ_BUFFER];
                while(1){
                    bzero(&buf,sizeof(buf));
                    ssize_t bytes_read = read(events[i].data.fd, buf,sizeof(buf));
                    if(bytes_read > 0){
                        printf("message from client fd %d: %s\n", events[i].data.fd, buf);
                        write(events[i].data.fd,buf,sizeof(buf));
                    }else if(bytes_read == -1 &&((errno == EAGAIN) || (errno ==EWOULDBLOCK))){
                        printf("finish reading once, errno: %d\n", errno);
                        break;
                    }else if(bytes_read == 0){
                        printf("EOF, client fd %d disconnected\n", events[i].data.fd);
                        close(events[i].data.fd);
                        break;
                    }
                }
            }else{
                printf("something else happened\n");
            }
        }
    }
    close(sockfd);
    return 0;
}