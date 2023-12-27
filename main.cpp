#include <iostream>
#include <optional>
#include <ostream>
#include "src/reciever.cpp"
#include "src/emitter.cpp"
#include "src/connecter.cpp"

int main(){
	Cryptor* crypt = new Cryptor();
	unsigned char key[] = "BQg8v6tFlsSRkvPA4m2qb3N7JQMLj2nX";
	crypt->set_symkey(&key[0]);
	//crypt->print_keys();

	std::string message = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Id aliquet risus feugiat in ante metus. Id donec ultrices tincidunt arcu non sodales neque sodales ut. Tellus id interdum velit laoreet id donec. Suspendisse sed nisi lacus sed viverra. Nullam ac tortor vitae purus faucibus ornare suspendisse sed. Eget dolor morbi non arcu risus quis varius. Urna cursus eget nunc scelerisque viverra mauris in aliquam sem. Eget gravida cum sociis natoque penatibus et magnis. Euismod quis viverra nibh cras pulvinar mattis nunc sed blandit.";
	std::string cipher = crypt->encrypt(message);
	std::optional<std::string> recv_data = crypt->decrypt(cipher);

	std::cout
		<< "MESSAGE CORRECT : " << (*recv_data == message) << std::endl
		<< "VALID : " << recv_data.has_value() << std::endl;

	/*Connecter connecter(crypt, "239.1.1.1", 9876);
	struct msocket sock_pair = connecter.get_socket();

	Emitter em(crypt, sock_pair.send);
	Reciever rc(crypt, sock_pair.recv);

	pthread_t em_id, rc_id;
	pthread_create(&em_id, NULL, &(Emitter::run), &em);
	pthread_create(&rc_id, NULL, &(Reciever::run), &rc);

	pthread_join(em_id, NULL);
	pthread_cancel(rc_id);*/


	delete crypt;
	return 0;
}