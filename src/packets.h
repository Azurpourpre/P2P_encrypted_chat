#include <cstdint>
#include "cryptor.cpp"

/*
    Packet ID :
    0b0 -> Hello
    0b1 -> Message
    0b1x -> Authentication with known public key
        0b10 -> Challenge
        0b11 -> Response
    0b1xx -> Key Request
        0b100 -> Add public key Request
*/

namespace Packets{
    enum packet_id : uint8_t {HELLO = 0b0, MESSAGE = 0b1, CHALLENGE = 0b10, RESPONSE = 0b11, ADD_KEY_REQUEST = 0b100};
    enum auth_type : uint8_t {CHALLENGE_RESPONSE = 0};

    typedef struct{
        const packet_id id = HELLO;
        auth_type auth;
    } Hello;

    typedef struct{
        const packet_id id = MESSAGE;
        char username[15];
        char message[1024];
    } Message;

    typedef struct{
        const packet_id id = CHALLENGE;
        char random[128];
        int n_zero;
    } Challenge;

    typedef struct{
        const packet_id id = RESPONSE;
        CryptoPP::byte hash[256];
    } Response;

    typedef struct{
        const packet_id id = ADD_KEY_REQUEST;
        char pubkey[440];
        char message[1024];
    } Add_key_request;
};