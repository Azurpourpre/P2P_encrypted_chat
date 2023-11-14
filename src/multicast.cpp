msocket_recv::msocket_recv(const char* ip_group, const int port){
    
    // INIT SOCKET
    this->fd_sock = socket(AF_INET, SOCK_DGRAM, 0);

    if(fd_sock < 0) err_exit(SOCKET_INIT_ERROR);

    u_int yes = 1;
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
}


void msocket_recv::recv(char* buffer, int bufsz){
    unsigned int addr_len = sizeof(this->addr);
    int nbytes = recvfrom(this->fd_sock, buffer, bufsz, 0, (struct sockaddr*)&addr, &addr_len);
    if(nbytes < 0) err_exit(SOCKET_RUNTIME_ERROR);
}

msocket_send::msocket_send(const char* ip_group, const int port){
    
    this->fd_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd_sock < 0) err_exit(SOCKET_INIT_ERROR);

    memset(&this->addr, 0, sizeof(this->addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip_group);
    addr.sin_port = htons(port);
}

void msocket_send::send(char* buffer, int bufsz){
    int nbytes = sendto(fd_sock, buffer, bufsz, 0, (struct sockaddr*)&addr, sizeof(addr));
    if(nbytes < 0) err_exit(SOCKET_RUNTIME_ERROR);
}