#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include "utils.h"

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

    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    bzero(&clnt_addr, sizeof(clnt_addr));
    int clnt_sockfd = accept(sockfd, (sockaddr*)&clnt_addr, &clnt_addr_len);
    errif(clnt_sockfd == -1, "socket accept error");
    printf("new client fd %d! IP: %s Port: %d\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
    while(1){
        char buf[1024];
        bzero(&buf, sizeof(buf));
        ssize_t read_bytes = read(clnt_sockfd, buf,sizeof(buf));//ssize_t等价于long，返回已读数据大小
        if(read_bytes > 0){
            std::cout << "message from client fd" << clnt_sockfd << ":" << buf << std::endl;
            write(clnt_sockfd, buf, sizeof(buf));//write back to client
        }else if(read_bytes == 0){//EOF
            std::cout << "client fd " << clnt_sockfd << "disconnected" <<std::endl;
            close(clnt_sockfd);
            break;
        }else if(read_bytes == -1){//error
            close(clnt_sockfd);
            errif(true, "socket read error");
        }
    }
    close(sockfd);
    return 0;
}