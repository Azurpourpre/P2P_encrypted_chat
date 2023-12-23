#ifndef _CONNECTER
#define _CONNECTER

#include "cryptor.cpp"
#include "multicast.cpp"

class Connecter{
    public:
        Connecter(Cryptor* cryptor, const char* ip_group, const int port);
        
        struct msocket get_socket();

        enum State {WAITING_CHALLENGE, WAITING_KEY};
    private:
        State state;
        Cryptor* cryptor;
        msocket_recv* s_recv;
        msocket_send* s_send;
};

Connecter::Connecter(Cryptor* cryptor, const char* ip_group, const int port){
    this->cryptor = cryptor;
    this->s_recv = new msocket_recv(ip_group, port);
    this->s_send = new msocket_send(ip_group, port);
}

struct msocket Connecter::get_socket(){
    struct msocket retval;
    retval.send = this->s_send;
    retval.recv = this->s_recv;
    return retval;
}

#endif