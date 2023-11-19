#include <iostream>
#include "src/multicast.h"
#include "src/packets.h"

int main(){
	Cryptor* crypt = new Cryptor();


	std::cout << "Public key size : " << crypt->get_pubkey().size() << std::endl;
	// SecByteBlock key = crypt->gen_AES_Key();
	// byte iv[AES::BLOCKSIZE];

	// std::string signed_msg = crypt->DSA_sign("Hello World !");
	// std::string ciphered = crypt->AES_encrypt(iv, signed_msg);

	// std::string clear = crypt->DSA_verify(crypt->AES_decrypt(iv,ciphered));
	// if (clear == ""){
	// 	crypt->get_vault().store(crypt->get_pubkey());
	// 	clear = crypt->DSA_verify(signed_msg);
	// }

	// std::cout << "Clear : " << clear << std::endl;

	return 0;
} 
