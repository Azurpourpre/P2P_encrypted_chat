#ifndef _CRYPTOR
#define _CRYPTOR

#include <cstring>
#include <exception>
#include <ios>
#include <optional>
#include <sys/types.h>
#include <unordered_set>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <cmath>
#include <vector>

#include "../lib/cryptopp/rsa.h"
#include "../lib/cryptopp/rijndael.h"
#include "../lib/cryptopp/osrng.h"
#include "../lib/cryptopp/filters.h"
#include "../lib/cryptopp/files.h"
#include "../lib/cryptopp/modes.h"

#include "error.cpp"

#define AES_KEY_SIZE 256/8
#define RSA_KEY_SIZE 4096/8
#define IV_VAL {38, 49, 234, 213, 173, 165, 254, 207, 18, 2, 96, 157, 117, 197, 215, 28}

using namespace CryptoPP;

typedef uint8_t RSA_Exp[RSA_KEY_SIZE];

class Vault{
    public:
        Vault();
        ~Vault();
        
        void store(RSA::PublicKey key);
        bool in(RSA::PublicKey key);
        std::unique_ptr<std::vector<Integer>> get();
    private:
        std::unordered_set<uint8_t*> data;
};

Vault::Vault(){
    std::streampos size;
    char* vaultMem;

    // Getting vault file in memory
    std::ifstream vaultFile(".keys/vault", std::ios::in | std::ios::binary | std::ios::ate);
    if(vaultFile.is_open()){

        size = vaultFile.tellg();
        vaultMem = new char[size];
        vaultFile.seekg(0, std::ios::beg);
        vaultFile.read((char*)vaultMem, size);
        vaultFile.close();

        // Parsing vault memory
        const unsigned int n_keys = *(unsigned int*)vaultMem;

        if((long unsigned int)size != sizeof(unsigned int) + (n_keys * RSA_KEY_SIZE)){
            err_exit(Errors::ENCRYPTION_RUNTIME_ERROR);
        }

        if(n_keys > 0){
            uint8_t* keys_arr = new uint8_t[n_keys * RSA_KEY_SIZE];
            memcpy(keys_arr, vaultMem + sizeof(unsigned int), n_keys * RSA_KEY_SIZE);
            for(unsigned int i = 0; i < n_keys; i++){
                this->data.insert(keys_arr + (i*RSA_KEY_SIZE));
            }
        }

        delete[] vaultMem;
    }
}

Vault::~Vault(){
    std::ofstream vaultFile(".keys/vault", std::ios::out | std::ios::binary);

    const unsigned int n_keys = this->data.size();
    vaultFile.write((char*)&n_keys, sizeof(unsigned int));

    for(auto i = this->data.begin(); i != this->data.end(); ++i){
        vaultFile.write((char*)*i, RSA_KEY_SIZE);
        delete[] *i;
    }

    vaultFile.close();
}

void Vault::store(RSA::PublicKey key){
    uint8_t* new_key = new RSA_Exp;
    key.GetModulus().Encode(new_key, RSA_KEY_SIZE, Integer::UNSIGNED);
    this->data.insert(new_key);
}

bool Vault::in(RSA::PublicKey key){
    for(auto i = this->data.begin(); i != this->data.end(); ++i){
        Integer cmp_exp = Integer(*i, RSA_KEY_SIZE, Integer::UNSIGNED);
        if(cmp_exp == key.GetModulus()){
            return true;
        }
    }
        

    return false;
}

std::unique_ptr<std::vector<Integer>> Vault::get(){
    std::unique_ptr<std::vector<Integer>> retval(new std::vector<Integer>);
    
    for(auto i = this->data.begin() ; i != this->data.end() ; ++i){
        retval->push_back(
            Integer(*i, RSA_KEY_SIZE, Integer::UNSIGNED)
        );
    }

    return retval;
}


class Cryptor{
    public:
        Cryptor();

        void set_symkey(uint8_t* key);
        void print_keys();

        std::string encrypt(std::string clear);
        std::optional<std::string> decrypt(std::string cipher);
    private:
        Vault vault;
        RSA::PublicKey pubKey;
        RSA::PrivateKey privKey;
        SecByteBlock symKey;

        std::string RSA_Sign(std::string message);
        std::optional<std::string> RSA_Verify(std::string signed_message);

        std::string AES_encrypt(std::string message);
        std::string AES_decrypt(std::string ciphered);
};

Cryptor::Cryptor(){
    try{
        ByteQueue pub_queue;
        FileSource pub_file(".keys/pub", true);
        pub_file.TransferTo(pub_queue);
        pub_queue.MessageEnd();
        this->pubKey.Load(pub_queue);

        ByteQueue priv_queue;
        FileSource priv_file(".keys/priv", true);
        priv_file.TransferTo(priv_queue);
        priv_queue.MessageEnd();
        this->privKey.Load(priv_queue);
    }
    catch(Exception& e){
        std::cerr << "Creating Keys !" << std::endl;
        // Generate key pair
        AutoSeededRandomPool rng;
        InvertibleRSAFunction params;
        params.GenerateRandomWithKeySize(rng, 8*RSA_KEY_SIZE);
        this->privKey = RSA::PrivateKey(params);
        this->pubKey = RSA::PublicKey(params);

        //Saving to file
        FileSink priv_file(".keys/priv");
        FileSink pub_file(".keys/pub");
        this->privKey.Save(priv_file);
        this->pubKey.Save(pub_file);

        //Adding self keys to vault
        this->vault.store(this->pubKey);
    }
}

void Cryptor::set_symkey(uint8_t* symkey){
    this->symKey = SecByteBlock(symkey, AES_KEY_SIZE);
}

void Cryptor::print_keys(){
    std::cout << "[PublicKey] e : " << this->pubKey.GetPublicExponent() << std::endl;
    std::cout << "[PublicKey] n : " << this->pubKey.GetModulus().ByteCount() << "B" << std::endl;
    std::cout << "[PrivateKey] d : " << this->privKey.GetPrivateExponent().ByteCount() << "B" << std::endl;
}

std::string Cryptor::RSA_Sign(std::string message){
    AutoSeededRandomPool rng;
    std::string signature;
    RSASSA_PKCS1v15_SHA_Signer signer(this->privKey);

    StringSource ss1(message, true,
        new SignerFilter(rng, signer, 
            new StringSink(signature)
        )
    );

    return message+signature;
}

std::optional<std::string> Cryptor::RSA_Verify(std::string signed_message){
    const std::unique_ptr<std::vector<Integer>> valid_modulus = this->vault.get();
    bool is_key_valid;

    for(auto i = valid_modulus->begin(); i != valid_modulus->end(); ++i){
        RSASSA_PKCS1v15_SHA_Verifier verifier(*i, this->pubKey.GetPublicExponent());
        is_key_valid = false;
        try{
            StringSource ss( signed_message, true,
                new SignatureVerificationFilter(
                    verifier,
                    new ArraySink(
                        (byte*)&is_key_valid, sizeof(is_key_valid)),
                        SignatureVerificationFilter::SIGNATURE_AT_END
                )
            );

            return signed_message.substr(0, signed_message.length() - 512);
        }
        catch(Exception& e){
            std::cerr << e.what() << std::endl;
        }
    }

    return std::nullopt;
}

std::string Cryptor::AES_encrypt(std::string message){
    CBC_Mode<AES>::Encryption e;
    byte raw_iv[AES::BLOCKSIZE] = IV_VAL; 
    SecByteBlock iv(raw_iv, AES::BLOCKSIZE);
    std::string retval;

    e.SetKeyWithIV(this->symKey, AES_KEY_SIZE, iv);
    StringSource s(message, true,
        new StreamTransformationFilter(e, 
            new StringSink(retval)
        )
    );

    return retval;
}

std::string Cryptor::AES_decrypt(std::string ciphered){
    byte raw_iv[AES::BLOCKSIZE] = IV_VAL; 
    SecByteBlock iv(raw_iv, AES::BLOCKSIZE);
    CBC_Mode<AES>::Decryption d;
    std::string retval;

    d.SetKeyWithIV(this->symKey, AES_KEY_SIZE, iv);

    StringSource s(ciphered, true,
        new StreamTransformationFilter(d,
            new StringSink(retval)
        )
    );

    return retval;
}

std::string Cryptor::encrypt(std::string clear){
    return 
        this->AES_encrypt(
            this->RSA_Sign(
                clear
            )
    );
}

std::optional<std::string> Cryptor::decrypt(std::string cipher){
    return 
        this->RSA_Verify(
        this->AES_decrypt(
            cipher
        )
    );
}

#endif