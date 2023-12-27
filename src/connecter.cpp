#ifndef _CONNECTER
#define _CONNECTER

#include "cryptor.cpp"
#include "multicast.cpp"
#include "packets.h"

class Connecter{
    public:
        Connecter(Cryptor* cryptor, const char* ip_group, const int port);
        
        struct msocket get_socket();
    private:
        Cryptor* cryptor;
        msocket_recv* s_recv;
        msocket_send* s_send;
};

Connecter::Connecter(Cryptor* cryptor, const char* ip_group, const int port){
    this->cryptor = cryptor;
    this->s_recv = new msocket_recv(ip_group, port);
    this->s_send = new msocket_send(ip_group, port);

    Packets::Hello init_packet;
    init_packet.auth = Packets::CHALLENGE_RESPONSE;
    this->s_send->send(&init_packet, sizeof(Packets::Hello));
}

struct msocket Connecter::get_socket(){
    struct msocket retval;
    retval.send = this->s_send;
    retval.recv = this->s_recv;
    return retval;
}

#endif