#ifndef _CONNECTER
#define _CONNECTER

#include "cryptor.cpp"
#include "error.cpp"
#include "multicast.cpp"
#include "packets.h"
#include <cstdint>
#include <cstring>
#include <optional>

class Connecter{
    public:
        Connecter(Cryptor* cryptor, const char* ip_group, const int port);
        
        struct msocket get_socket();

        enum connect_status {SUCCESS, FIRST, FAILED};
        connect_status connect(std::string message);

        void resp_Hello(const Packets::auth_type auth);
        void resp_Challenge(const Packets::Challenge chall, const std::string message);
        void resp_Response(const Packets::Response resp);

        void send_keys();
    private:
        Cryptor* cryptor;
        msocket_recv* s_recv;
        msocket_send* s_send;

        const std::string to_sign = "zBRlrwbR8T796cbwverWY4K0MYt5FgL6rkCQhWjqigtpBLTjktp4E31AE2gW4CorSnpO4gRgHH1LsuGGaSWMCV2qY1Au5JHfqq3wljt4iIlYJifnGvpJAIZaRVsBTMMrIYAabILcuHKHEHYQobFsviIJPFSilpSezi7m0OIhlxQOpMJjV2njFqTPvZUUX5PJBzHMwoRHoFa2tB6clHOaUqwtvRZxSxWL978dln24NWfUXyrJ3WoMLSdD8qfXWn8gY9z5gqYLhq92EOlLu2KHzOWOP2p23ewwdHwjzzfXaAlJz021B8GlBs5WJLafOtFATVZLSHgtX0hWaUJDLPo3XKF6QWprqyTprh9ZSC4hGi0ocquNxeWjY9QIzIAQeGhhe8WxP8O0cxPHDljRtaJUYsffPK5CznlxXEQRZ8esMKSIrMmN3VkbPdhKcSGWR49COA7gFtM1DotBSfpTAbLycaF4aCYPNtx0OPWQTMDm3rOZjjzVFlrHoKzo6Ky1U1God7jk0g7mHWidecKSrNoZbFOjbuMfHGOcB4q4VNWkO7WEuGpx4jExwXAKSjXXbIOClz8wJDiBTAHGkHvCNA7dSnmsflYDk046sjY1DX0ILehqQkBSwTbEv0uWWd4ayuhs4oBeODF2A8qLr4D2zKRc4FchPIFEvbURBt0ei158Qz68qD7CAg0wUAtD9ufHA53FxGU8OZ4HPm7c7T0goRnenUvSfByvM2rHh2A9v5apCHET1WBu1quHPoBuhrkQ3oAKn3Eya4rH8lXJsPbJAzsjE6O3qW8pSzscEsRhz05qizXgi2aNtZRRcsKwIvCJCzVH6Y8aFwvbAeIRGtEqTBol84UizxvkxYeH8KGEA8z2ElrkA2laDDq2lhraOoGx7J50Sn0MS3Wsg0QStGgsS3Pn59BUj9sh4fFEr0KyjEfYfq9o2igxT2k0HTnTWJ75xEaup6UZboaCp1TpErAZZOboslm4hGvCcAeemegGnqDMlDl6YZfC3YRhfQZTFOx2QTpn";
};

Connecter::Connecter(Cryptor* cryptor, const char* ip_group, const int port){
    this->cryptor = cryptor;
    this->s_recv = new msocket_recv(ip_group, port);
    this->s_send = new msocket_send(ip_group, port);
}

Connecter::connect_status Connecter::connect(const std::string message){
    uint8_t buffer[MAX_PACKET_SIZE];

    // Send Hello
    Packets::Hello init_packet;
    init_packet.auth = Packets::AUTH_CHALLENGE_RESPONSE;
    this->s_send->send(&init_packet, sizeof(Packets::Hello));
    std::cout << "Sent Hello Packet !" << std::endl;
    
    // resp_Challenge
    this->s_recv->recv(buffer, MAX_PACKET_SIZE);
    Packets::packet_id* pkt_id = (Packets::packet_id*)buffer;
    switch(*pkt_id){
        case Packets::ID_HELLO:
            // We were the first to connect
            return FIRST;
        case Packets::ID_CHALLENGE:
            //We need to solve the challenge = continue
            break;
        default:
            return FAILED;
    }
    resp_Challenge(*(Packets::Challenge*)buffer, message);
    

    // Resp Valid
    do{
        this->s_recv->recv(buffer, MAX_PACKET_SIZE);
        pkt_id = (Packets::packet_id*) buffer;
    }while(*pkt_id != Packets::ID_VALID);
    
    std::cout << "[VALID] Got symkey !" << std::endl;
    Packets::Valid pkt = *(Packets::Valid*)buffer;
    this->cryptor->set_symkey(std::string((char*)pkt.symkey, RSA_KEY_SIZE));
    
    return SUCCESS;
}

void Connecter::resp_Hello(const Packets::auth_type auth){
    switch(auth){
        case Packets::AUTH_CHALLENGE_RESPONSE:
            Packets::Challenge resp;
            std::string challenge = this->cryptor->RSA_Sign(this->to_sign);
            memcpy(resp.approved_sign, challenge.c_str(), RSA_KEY_SIZE);
            this->s_send->send(&resp, sizeof(Packets::Challenge));
            std::cout << "[HELLO] Challenge sent !" << std::endl;
            break;
/*        default:
            err_exit(Errors::SOCKET_RUNTIME_ERROR);
*/
    }
}

void Connecter::resp_Challenge(const Packets::Challenge chall, const std::string message){
    Packets::Response resp;

    memcpy(resp.pubkey, this->cryptor->get_pubkey(), RSA_KEY_SIZE);
    strncpy((char*)resp.message, message.c_str(), 1023);
    const std::string s_sign = this->cryptor->RSA_Sign(
        std::string((char*)chall.approved_sign, RSA_KEY_SIZE)
    );
    memcpy(resp.approved_sign, chall.approved_sign, RSA_KEY_SIZE);
    memcpy(resp.sign, s_sign.c_str(), RSA_KEY_SIZE);

    this->s_send->send(&resp, sizeof(Packets::Response));

    std::cout << "[CHALLENGE] Response sent !" << std::endl;
}

void Connecter::resp_Response(const Packets::Response resp){
    std::string s_as((char*)resp.approved_sign, RSA_KEY_SIZE);
    std::string s_si((char*)resp.sign, RSA_KEY_SIZE);
    Integer foreign_pubkey(resp.pubkey, RSA_KEY_SIZE, Integer::UNSIGNED);

    std::optional<std::string> step1 = this->cryptor->RSA_Verify(s_as + s_si, foreign_pubkey);
    if(step1 != std::nullopt){
        std::optional<std::string> step2 =  this->cryptor->RSA_Verify(this->to_sign + *step1);
        if(step2 != std::nullopt){
            RSA_Exp new_key;
            memcpy(&new_key, &resp.pubkey, RSA_KEY_SIZE);
            this->cryptor->get_vault()->store(new_key);

            Packets::Valid resp_pkt;
            uint8_t* c_symkey = this->cryptor->get_symkey(resp.pubkey);
            memcpy(resp_pkt.symkey, c_symkey, RSA_KEY_SIZE);
            
            this->s_send->send(&resp_pkt, sizeof(Packets::Valid));

            delete[] c_symkey;

            this->send_keys();
        }
        else{
            std::cerr << "Invalid Response ! (Failed at step 2)" << std::endl;
        }
    }
    else{
        std::cerr << "Invalid Response ! (Failed at step 2)" << std::endl;
    }
}

void Connecter::send_keys(){
    // Sends public Key Encrypted with symkey
    std::string pubkey((char*)this->cryptor->get_pubkey(), RSA_KEY_SIZE);
    std::string enc_pubkey = this->cryptor->AES_encrypt(pubkey);

    std::cout << "Encrypted key size : " << enc_pubkey.length() << "B" << std::endl;
    
    Packets::Keys pkt;
    memcpy(pkt.mykey, enc_pubkey.data(), RSA_KEY_SIZE + 16);

    this->s_send->send(&pkt, sizeof(Packets::Keys));
}

struct msocket Connecter::get_socket(){
    struct msocket retval;
    retval.send = this->s_send;
    retval.recv = this->s_recv;
    return retval;
}

#endif