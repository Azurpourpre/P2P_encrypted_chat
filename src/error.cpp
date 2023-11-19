#ifndef _ERROR
#define _ERROR

#include <iostream>

enum error_code {NO_ERROR = 0, SOCKET_INIT_ERROR = 18, SOCKET_RUNTIME_ERROR = 19, ENCRYPTION_ERROR = 1};

void err_exit(error_code err){
    switch(err){
        case NO_ERROR:
            std::cerr << "Emergency Exit" << std::endl;
            break;
        case SOCKET_INIT_ERROR:
            std::cerr << "Error on socket initialization" << std::endl;
            break;
    }

    exit(err);
}

#endif