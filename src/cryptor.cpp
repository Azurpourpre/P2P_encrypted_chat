#ifndef _CRYPTOR
#define _CRYPTOR

#include <unordered_set>
#include <fstream>
#include <iostream>
#include "error.cpp"
#include "../lib/cryptopp/dsa.h"
#include "../lib/cryptopp/osrng.h"
#include "../lib/cryptopp/base64.h"
#include "../lib/cryptopp/files.h"
#include "../lib/cryptopp/rijndael.h"
#include "../lib/cryptopp/gcm.h"
#include "../lib/cryptopp/filters.h"

#define AES_KEY_LENGTH 32
#define TAG_SIZE 12

using namespace CryptoPP;

class Vault{
    public:
        Vault();
        ~Vault();
        void store(std::string key);
        const std::unordered_set<DSA::PublicKey*> get();
    private:
        std::unordered_set<DSA::PublicKey*> data;
        void append(std::string key);
};

class Cryptor{
    public:
        Cryptor();
        std::string get_pubkey();
        Vault& get_vault();

        std::string DSA_sign(std::string message);
        std::string DSA_verify(std::string message);

        SecByteBlock& gen_AES_Key();
        void import_AES_Key(byte* new_key);
        std::string AES_encrypt(byte* iv, std::string message);
        std::string AES_decrypt(byte* iv, std::string ciphered);
        
    
    private:
        DSA::PrivateKey DSA_privkey;
        DSA::PublicKey DSA_pubkey;

        SecByteBlock AES_key;

        Vault vault;
};

Vault::Vault(){
    std::ifstream infile;
    infile.open(".vault");

    std::string decoded;
    Base64Decoder decoder(new StringSink(decoded));
    for(std::string line; getline(infile, line) ; ){
        decoder.Put((unsigned char*)line.data(), line.size());
        decoder.MessageEnd();

        this->append(decoded);
        decoded = "";
    }
}

Vault::~Vault(){
    for(const auto& key : this->data){
        delete key;
    }
}

void Vault::store(std::string key){
    std::string encoded;
    Base64Encoder encoder(new StringSink(encoded), false);
    encoder.Put((unsigned char*)key.data(), key.size());
    encoder.MessageEnd();

    std::ofstream outfile;
    outfile.open(".vault", std::ofstream::app);
    outfile << encoded << std::endl;
    outfile.close();

    this->append(key);
}

const std::unordered_set<DSA::PublicKey*> Vault::get(){
    return this->data;
}

void Vault::append(std::string key){
    DSA::PublicKey* new_pubkey = new DSA::PublicKey();
    new_pubkey->Load(StringStore(key).Ref());
    data.insert(new_pubkey);
}



Cryptor::Cryptor(){
    Vault vault = Vault();

    std::ifstream pubkey_file, privkey_file;
    privkey_file.open(".keys/privkey");

    if(privkey_file.is_open()){
        this->DSA_privkey.Load(FileSource(privkey_file, true).Ref());
        this->DSA_pubkey.AssignFrom(DSA_privkey);
    }
    else {
        privkey_file.close();
        AutoSeededRandomPool rng;
        //Recreate the DSA Keys


        this->DSA_privkey.GenerateRandomWithKeySize(rng, 1024);
        this->DSA_pubkey.AssignFrom(DSA_privkey);


        if(!DSA_privkey.Validate(rng,3) || !DSA_pubkey.Validate(rng, 3))
            throw std::runtime_error("DSA Key generation failed");


        std::ofstream privkey_file;

        privkey_file.open(".keys/privkey");
        DSA_privkey.Save(FileSink(privkey_file).Ref());
        privkey_file.close();
    }
}

std::string Cryptor::get_pubkey(){
    std::string retval;
    DSA_pubkey.Save(StringSink(retval).Ref());
    return retval;
}

Vault& Cryptor::get_vault(){ return this->vault; }

std::string Cryptor::DSA_sign(std::string message){
    AutoSeededRandomPool rng;
    std::string signature;
    DSA::Signer signer(DSA_privkey);

    StringSource ss(message, true,
        new SignerFilter(rng, signer, 
            new StringSink(signature)));

    return message+signature;
}

std::string Cryptor::DSA_verify(std::string message){
    bool result = false;
    for(PublicKey* candidate: this->vault.get()){
        DSA::Verifier verifier(*candidate);

        
        StringSource ss(message, true,
            new SignatureVerificationFilter(
                verifier,
                new ArraySink((byte*) &result, sizeof(result)), 
                8 | 0 /* PUT RESULT | SIGNATURE_AT_THE_END */
        ));

        if(result)
            return message.substr(0, message.length() - 40);
    }
    
    std::cerr << "Invalid Signature" << std::endl;
    return "";
}

SecByteBlock& Cryptor::gen_AES_Key(){
    AutoSeededRandomPool rng;
    AES_key = SecByteBlock(AES_KEY_LENGTH);
    rng.GenerateBlock(AES_key, AES_KEY_LENGTH);

    return this->AES_key;
}

void Cryptor::import_AES_Key(byte* new_key){
    this->AES_key = SecByteBlock(new_key, AES_KEY_LENGTH);
}

std::string Cryptor::AES_encrypt(byte* iv, std::string message){
    AutoSeededRandomPool rng;
    std::string cipher;
    rng.GenerateBlock(iv, AES::BLOCKSIZE);

    try{
        GCM<AES>::Encryption e;
        e.SetKeyWithIV(AES_key, AES_key.size(), iv, AES::BLOCKSIZE);

        StringSource ss(message, true,
            new AuthenticatedEncryptionFilter(e, 
                new StringSink(cipher), false, TAG_SIZE));
    }
    catch(Exception& e){
        std::cerr << e.what() << std::endl;
        err_exit(ENCRYPTION_ERROR);
    }

    return cipher;
}

std::string Cryptor::AES_decrypt(byte* iv, std::string ciphered){
    std::string clear;
    try{
        GCM<AES>::Decryption d;
        d.SetKeyWithIV(AES_key, AES_key.size(), iv, AES::BLOCKSIZE);

        AuthenticatedDecryptionFilter df(d,
            new StringSink(clear),
            16 | 0 /*DEFAULT FLAGS*/, TAG_SIZE);
        
        StringSource ss2(ciphered, true,
            new Redirector(df));
        
        if(df.GetLastResult() == false)
            throw new std::runtime_error("Error on intgrity when decrypting");
    }
    catch(Exception& e){
        std::cerr << e.what() << std::endl;
        err_exit(ENCRYPTION_ERROR);
    }

    return clear;
}

#endif