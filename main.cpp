#include <iostream>
#include "src/reciever.cpp"
#include "src/emitter.cpp"



int main(){
	Cryptor* crypt = new Cryptor();

	Emitter em(crypt, "127.0.0.1", 12345);
	Reciever rc(crypt, "239.1.1.1", 9876);

	pthread_t em_id, rc_id;
	pthread_create(&em_id, NULL, &(Emitter::run), &em);
	pthread_create(&rc_id, NULL, &(Reciever::run), &rc);

	pthread_join(em_id, NULL);
	pthread_cancel(rc_id);

	delete crypt;
	return 0;
} 
