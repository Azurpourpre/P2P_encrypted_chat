#ifndef _CRYPTOR
#define _CRYPTOR

#include <set>
#include <fstream>
#include "../lib/cryptopp/dsa.h"
#include "../lib/cryptopp/osrng.h"
#include "../lib/cryptopp/base64.h"

using namespace CryptoPP;

class Vault{
    public:
        Vault();
        void store(std::string key);
        const std::set<DSA::PublicKey*> get();
    private:
        std::set<DSA::PublicKey*> data;
        void append(std::string key);
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

void Vault::store(std::string key){
    std::string encoded;
    Base64Encoder encoder(new StringSink(encoded), false);
    encoder.Put((unsigned char*)key.data(), key.size());
    encoder.MessageEnd();

    std::ofstream outfile;
    outfile.open(".vault");
    outfile << encoded << std::endl;
    outfile.close();

    this->append(key);

}

const std::set<DSA::PublicKey*> Vault::get(){
    return this->data;
}

void Vault::append(std::string key){
    DSA::PublicKey* new_pubkey = new DSA::PublicKey();
    new_pubkey->Load(StringStore(key).Ref());
    data.insert(new_pubkey);
}

#endif