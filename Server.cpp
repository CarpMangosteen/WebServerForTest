#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>

int main(){
    int sockfd = socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in serv_addr;//结构体sockaddrin包含在inet.h中，包含了地址族端口号与ip地址

    bzero(&serv_addr,sizeof(serv_addr));//初始化

    serv_addr.sin_family = AF_INET;//协议族ip4
    serv_addr.sin_addr.s_addr= inet_addr("127.0.0.1");//IP地址，这一串没太看懂，先用着
    serv_addr.sin_port = htons(8888);

    bind(sockfd,(sockaddr*)&serv_addr,sizeof(serv_addr));//bind函数，挺有意思的

    listen(sockfd, SOMAXCONN);//监听sockfd端口

    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    bzero(&clnt_addr, sizeof(clnt_addr));
    int clnt_sockfd = accept(sockfd, (sockaddr*)&clnt_addr, &clnt_addr_len);
    std::cout << "new client fd " << clnt_sockfd << "! IP: " << inet_ntoa(clnt_addr.sin_addr) << " Port: " << ntohs(clnt_addr.sin_port) << std::endl;
    return 0;
}