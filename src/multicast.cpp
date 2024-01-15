#ifndef _MULTICAST
#define _MULTICAST

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <cstring>
#include <vector>

#include "error.cpp"

class msocket_recv{
    public:
        msocket_recv(const char* ip_group, const int port);
        void recv(void* buffer, int bufsz);
    private:
        int fd_sock;
        struct sockaddr_in addr;
        std::vector<in_addr_t> if_ip;
};

class msocket_send{
    public:
        msocket_send(const char* ip_group, const int port);
        ~msocket_send();
        void send(void* buffer, int bufsz);
    private:
        int fd_sock;
        struct sockaddr_in addr;
        char* ip_group;
        int port;
};

struct msocket{
    msocket_recv* recv;
    msocket_send* send;
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

    // Get my ip address(es)

    struct ifaddrs* tmp, *myif;
    getifaddrs(&myif);
    tmp = myif;
    while(tmp){
        if(tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET){
            const struct sockaddr_in* tmp_skr = (const sockaddr_in*)tmp->ifa_addr;
            this->if_ip.push_back(tmp_skr->sin_addr.s_addr);
        }
        tmp = tmp->ifa_next;
    }
    freeifaddrs(myif);
}


void msocket_recv::recv(void* buffer, int bufsz){
    struct sockaddr_in recv_addr;
    unsigned int addr_len = sizeof(recv_addr);
    bool found = false;

    while(found == false){
        int nbytes = recvfrom(this->fd_sock, buffer, bufsz, 0, (struct sockaddr*)&recv_addr, &addr_len);
        if(nbytes < 0) err_exit(SOCKET_RUNTIME_ERROR);

        /* Check if address is mine */
        bool local = false;

        for(auto i = this->if_ip.begin() ; i != this->if_ip.end() && local == false; ++i){
            if (*i == recv_addr.sin_addr.s_addr)
                local = true;
        }

        if(local == false){
            found = true;
        }
    }

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

msocket_send::~msocket_send(){
    delete[] this->ip_group;
}

void msocket_send::send(void* buffer, int bufsz){
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(this->port);
    dest.sin_addr.s_addr = inet_addr(this->ip_group);

    int nbytes = sendto(fd_sock, buffer, bufsz, 0, (struct sockaddr*)&dest, sizeof(dest));
    if(nbytes < 0) err_exit(SOCKET_RUNTIME_ERROR);
}

#endif