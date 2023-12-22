#ifndef _RECIEVER
#define _RECIEVER

#include <iostream>
#include "multicast.cpp"
#include "cryptor.cpp"
#include "packets.h"
#include "error.cpp"

class Reciever{
    public:
        Reciever(Cryptor* cryptor, const char* ip, const int port);
        ~Reciever();
        static void* run(void* self);

        void doHello(const Packets::Hello data);
        void doResponse(const Packets::Response data);
        void doMessage(const Packets::Message data);
        void doAddKey(const Packets::Add_key_request data);
    private:
        msocket_recv* socket;
        Cryptor* cryptor;
};


Reciever::Reciever(Cryptor* cryptor, const char* ip, const int port){
    this->socket = new msocket_recv(ip, port);
    this->cryptor = cryptor;
}

Reciever::~Reciever(){
    delete this->socket;
}

void* Reciever::run(void* pself){
    Reciever* self = (Reciever*)pself;
    while(1){
        char data[MAX_PACKET_SIZE];
        self->socket->recv(data, MAX_PACKET_SIZE);

        const Packets::packet_id pid = *(Packets::packet_id*)data; // Packets always starts with packet id
        switch(pid){
            case Packets::HELLO:
                self->doHello(*(const Packets::Hello*)data);
                break;
            case Packets::MESSAGE:
                self->doMessage(*(const Packets::Message*)data);
                break;
            case Packets::RESPONSE:
                self->doResponse(*(const Packets::Response*)data);
                break;
            case Packets::ADD_KEY_REQUEST:
                self->doAddKey(*(const Packets::Add_key_request*)data);
                break;
            default:
                break;
        }
    }
    return NULL;
}

void Reciever::doHello(const Packets::Hello data){
    switch(data.auth){
        case Packets::CHALLENGE_RESPONSE:
            std::cout << "[HELLO] Challenge Response" << std::endl;
            break;
        default:
            err_exit(SOCKET_RUNTIME_ERROR);
    }
}

void Reciever::doMessage(const Packets::Message data){
    std::cout << "[MESSAGE] " << data.username << " : " << data.message << std::endl;
}

void Reciever::doResponse(const Packets::Response data){
    std::cout << "[RESPONSE] {Random : " << std::hex << data.random << ", Signature : " << std::hex << data.hash << "}" << std::endl;
}

void Reciever::doAddKey(const Packets::Add_key_request data){
    std::cout << "[ADD KEY] {Pubkey : " << std::hex << data.pubkey << ", Message : " << data.message << "}" << std::endl;
}

#endif