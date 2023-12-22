#ifndef _MULTICAST
#define _MULTICAST

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include "error.cpp"

class msocket_recv{
    public:
        msocket_recv(const char* ip_group, const int port);
        void recv(char* buffer, int bufsz);
    private:
        int fd_sock;
        struct sockaddr_in addr;
};

class msocket_send{
    public:
        msocket_send(const char* ip_group, const int port);
        void send(char* buffer, int bufsz);
    private:
        int fd_sock;
        struct sockaddr_in addr;
        char* ip_group;
        int port;
};


msocket_recv::msocket_recv(const char* ip_group, const int port){
    
    // INIT SOCKET
    this->fd_sock = socket(AF_INET, SOCK_DGRAM, 0);

    if(fd_sock < 0) err_exit(SOCKET_INIT_ERROR);

    unsigned int yes = 1;
    if(
        setsockopt(fd_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes)) < 0
    ) err_exit (SOCKET_INIT_ERROR);

    // INIT SOCKADDR
    memset(&this->addr, 0, sizeof(this->addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    // BIND AND JOIN MULTICAST
    if(
        bind(fd_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0
    ) err_exit(SOCKET_INIT_ERROR);

    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(ip_group);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if(
        setsockopt(fd_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) < 0
    ) err_exit(SOCKET_INIT_ERROR);
}


void msocket_recv::recv(char* buffer, int bufsz){
    unsigned int addr_len = sizeof(this->addr);
    int nbytes = recvfrom(this->fd_sock, buffer, bufsz, 0, (struct sockaddr*)&addr, &addr_len);
    if(nbytes < 0) err_exit(SOCKET_RUNTIME_ERROR);
}

msocket_send::msocket_send(const char* ip_group, const int port){
    this->ip_group = new char[16];
    strncpy(this->ip_group, ip_group, 15);
    memcpy(&this->port, &port, sizeof(int));

    this->fd_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd_sock < 0) err_exit(SOCKET_INIT_ERROR);

    memset(&this->addr, 0, sizeof(this->addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip_group);
    addr.sin_port = 0;
}

void msocket_send::send(char* buffer, int bufsz){
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(this->port);
    dest.sin_addr.s_addr = inet_addr(this->ip_group);

    int nbytes = sendto(fd_sock, buffer, bufsz, 0, (struct sockaddr*)&dest, sizeof(dest));
    if(nbytes < 0) err_exit(SOCKET_RUNTIME_ERROR);
}

#endif