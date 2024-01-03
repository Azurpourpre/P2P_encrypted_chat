#include <iostream>
#include <optional>
#include <ostream>
#include "src/error.cpp"
#include "src/packets.h"
#include "src/reciever.cpp"
#include "src/emitter.cpp"
#include "src/connecter.cpp"

int main(){
	Cryptor* crypt = new Cryptor();

	Connecter connecter(crypt, "239.69.69.69", 9876);

	Emitter em(crypt, connecter.get_socket().send);
	Reciever rc(crypt, &connecter);

	Packets::Hello hello_pkt;
	hello_pkt.auth = Packets::AUTH_CHALLENGE_RESPONSE;
	switch(connecter.connect("Hello ! I want to connect to this network :)")){
		case Connecter::FIRST:
			crypt->gen_symkey();
			rc.doHello(hello_pkt);
			break;
		case Connecter::SUCCESS:
			break;
		case Connecter::FAILED:
			err_exit(Errors::SOCKET_INIT_ERROR);
			break;
	}

	crypt->print_keys();

	pthread_t em_id, rc_id;
	pthread_create(&em_id, NULL, &(Emitter::run), &em);
	pthread_create(&rc_id, NULL, &(Reciever::run), &rc);

	pthread_join(em_id, NULL);
	pthread_cancel(rc_id);


	delete crypt;
	return 0;
}