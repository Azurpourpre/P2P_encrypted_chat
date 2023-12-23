#ifndef _EMITTER
#define _EMITTER

#include "packets.h"

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
    int iter = 0;

    do{
        std::cout << "{iter : " << iter++ << "}" << std::endl;
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
    else if(input.compare("rename") == 0){
        std::cout << "Enter your new username : ";
        std::getline(std::cin, this->username);
    }
}

void Emitter::doMessage(const std::string message){
    Packets::Message sendval;
    strncpy(sendval.username, this->username.c_str(), 15);
    strncpy(sendval.message, message.c_str(), 1024);

    this->socket->send((char*)&sendval, sizeof(Packets::Message));
}

#endif