#ifndef _EMITTER
#define _EMITTER

#include <cstring>
#include "packets.h"
#include "cryptor.cpp"
#include "multicast.cpp"

#define MIN(a, b) (a < b) ? a : b

class Emitter{
    public:
        Emitter(Cryptor* cryptor, msocket_send* socket);
        ~Emitter();
        static void* run(void* pself);

        void parseCommand(const std::string input);
        void doMessage(const std::string message);
    private:
        msocket_send* socket;
        Cryptor* cryptor;
        std::string username;
        bool exit;
};

Emitter::Emitter(Cryptor* cryptor, msocket_send* socket){
    this->socket = socket;
    this->cryptor = cryptor;
    this->exit = false;
    this->username = "Anon";
}

Emitter::~Emitter(){
    delete this->socket;
}

void* Emitter::run(void* pself){
    Emitter* self = (Emitter*)pself;
    std::string input;

    do{
        std::cout << "> ";
        std::getline(std::cin, input);

        if(input[0] == '\n' || input[0] == '\x00'){
            /* SKIP */
        }
        else if(input[0] == '/'){
            self->parseCommand(input.substr(1));
        }
        else{
            self->doMessage(input);
        }

    }while(self->exit == false);

    return NULL;
}

void Emitter::parseCommand(const std::string input){
    std::cout << "[COMMAND] " << input << std::endl;

    if(input.compare("quit") == 0){
        this->exit = true;
    }
    else if(input.compare("nick") == 0){
        std::cout << "Enter your new username : ";
        std::getline(std::cin, this->username);
        this->username = this->username.substr(0, 15);
    }
    else if(input.compare("vault_sz") == 0){
        std::cout << "Your vault has " << this->cryptor->get_vault()->size() << " keys !" << std::endl;
    }
}

void Emitter::doMessage(const std::string message){
    Packets::Message sendval;
    std::string cropped_message = message.substr(0, 1024 + RSA_KEY_SIZE);
    std::string to_send = this->cryptor->encrypt(cropped_message);

    std::cerr << "Cropped message length : " << cropped_message.length() << std::endl
        << "Ciphertext length : " << to_send.length() << std::endl;

    memset(sendval.username, 0, 16);
    strncpy((char*)sendval.username, this->username.c_str(), 15);
    memset(sendval.signed_message, 0, 1024 + RSA_KEY_SIZE + AES::BLOCKSIZE);
    memcpy(sendval.signed_message, to_send.c_str(), to_send.length());
    sendval.sz = to_send.length();

    this->socket->send(&sendval, sizeof(Packets::Message));
}

#endif