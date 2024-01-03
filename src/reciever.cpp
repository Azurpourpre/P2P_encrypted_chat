#ifndef _RECIEVER
#define _RECIEVER

#include <iostream>
#include <memory>
#include <optional>
#include "multicast.cpp"
#include "cryptor.cpp"
#include "packets.h"
#include "error.cpp"
#include "connecter.cpp"

class Reciever{
    public:
        Reciever(Cryptor* cryptor, Connecter* conn);
        ~Reciever();
        static void* run(void* self);

        void doHello(const Packets::Hello data);
        void doResponse(const Packets::Response data);
        void doMessage(const Packets::Message data);
    private:
        msocket_recv* socket;
        Connecter* conn;
        Cryptor* cryptor;
};


Reciever::Reciever(Cryptor* cryptor, Connecter* conn){
    this->socket = conn->get_socket().recv;
    this->conn = conn;
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
            case Packets::ID_HELLO:
                self->doHello(*(const Packets::Hello*)data);
                break;
            case Packets::ID_MESSAGE:
                self->doMessage(*(const Packets::Message*)data);
                break;
            case Packets::ID_RESPONSE:
                self->doResponse(*(const Packets::Response*)data);
                break;
            default:
                break;
        }
    }
    return NULL;
}

void Reciever::doHello(const Packets::Hello data){
    std::cout << "[HELLO GOT]" << std::endl;
    conn->resp_Hello(data.auth);
}

void Reciever::doMessage(const Packets::Message data){
    std::string s_smsg((char*)data.signed_message, data.sz);
    std::optional<std::string> s_dec = this->cryptor->decrypt(s_smsg);
    if(s_dec)
        std::cout << "[MESSAGE] " << data.username << " : " << *s_dec << std::endl;
    else
        std::cout << "[MESSAGE] Cannot decipher message :(" << std::endl;
}

void Reciever::doResponse(const Packets::Response data){
    std::cout << "[RESPONSE] {Message : " << data.message << "}" << std::endl;
    this->conn->resp_Response(data);
}

#endif