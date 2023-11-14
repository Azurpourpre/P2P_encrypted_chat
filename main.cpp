#include <iostream>
#include "src/multicast.h"

int main(){
	msocket socket = msocket("224.0.0.30", 1337, 1338);

	char msg_buffer[500];
	socket.recv->recv(msg_buffer, 500);

	return 0;
} 
