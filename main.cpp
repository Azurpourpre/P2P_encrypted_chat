#include <iostream>
#include "src/multicast.h"
#include "src/cryptor.cpp"

int main(){
	Cryptor* crypt = new Cryptor();

	crypt->get_vault().store(crypt->get_pubkey());

	SecByteBlock key = crypt->gen_AES_Key();

	std::cout << key.data() << std::endl;

	return 0;
} 
