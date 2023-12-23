#include <iostream>
#include "src/reciever.cpp"
#include "src/emitter.cpp"
#include "src/connecter.cpp"

int main(){
	Cryptor* crypt = new Cryptor();

	Connecter connecter(crypt, "239.1.1.1", 9876);
	struct msocket sock_pair = connecter.get_socket();

	Emitter em(crypt, sock_pair.send);
	Reciever rc(crypt, sock_pair.recv);

	pthread_t em_id, rc_id;
	pthread_create(&em_id, NULL, &(Emitter::run), &em);
	pthread_create(&rc_id, NULL, &(Reciever::run), &rc);

	pthread_join(em_id, NULL);
	pthread_cancel(rc_id);

	delete crypt;
	return 0;
} 
