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
};

#include "multicast.cpp"

class msocket{
    public:
        msocket(const char* ip_group, const int r_port, const int s_port){
            msocket_recv r_tmp = msocket_recv(ip_group, r_port);
            this->recv = (msocket_recv*)malloc(sizeof(msocket_recv));
            memcpy((void*)this->recv, (const void*)&r_tmp, sizeof(msocket_recv));

            msocket_send s_tmp = msocket_send(ip_group, s_port);
            this->send = (msocket_send*)malloc(sizeof(msocket_recv));
            memcpy((void*)this->send, (const void*)&s_tmp, sizeof(msocket_send));
        }

    msocket_send* send;
    msocket_recv* recv;
};

#endif