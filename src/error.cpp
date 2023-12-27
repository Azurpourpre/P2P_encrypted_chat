#ifndef _ERROR
#define _ERROR

#include <iostream>

namespace Errors{
    enum error_code {NO_ERROR, SOCKET_INIT_ERROR, SOCKET_RUNTIME_ERROR, ENCRYPTION_INIT_ERROR, ENCRYPTION_RUNTIME_ERROR, MEMORY_ERROR, VAULT_ERROR};
}

using namespace Errors;
void err_exit(error_code err){
    switch(err){
        case NO_ERROR:
            std::cerr << "Emergency Exit" << std::endl;
            break;
        case SOCKET_INIT_ERROR:
            std::cerr << "Error on socket initialization" << std::endl;
            break;
        case SOCKET_RUNTIME_ERROR:
            std::cerr << "Error on runtime because of socket" << std::endl;
            break;
        case ENCRYPTION_INIT_ERROR:
            std::cerr << "Error when initializing encryption" << std::endl;
            break;
        case ENCRYPTION_RUNTIME_ERROR:
            std::cerr << "Error when performing encryption" << std::endl;
            break;
        case MEMORY_ERROR:
            std::cerr << "Error on memory allocation. Buy more ram looser" << std::endl;
            break;
        case VAULT_ERROR:
            std::cerr << "Vault file not found" << std::endl;
            break;
    }

    exit(err);
}

#endif