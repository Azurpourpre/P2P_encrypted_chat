#ifndef _PACKETS
#define _PACKETS

#include "cryptor.cpp"
#include <cstdint>

/*
    Packet ID :
    0x0? -> General Purpose Messages
        0b0 -> Don't assign
        0b1 -> Message
        0b1?? -> Control Messages
            0b100 -> Hello
            0b101 -> Keys
    0x1? -> Authentication with known public key
        0x10 -> Challenge
        0x11 -> Response
        0x12 -> Valid
    
*/

namespace Packets{
    enum packet_id : uint8_t {ID_MESSAGE = 0b1, ID_HELLO = 0b100, ID_KEYS = 0b101, ID_ANSWER_KEY = 0b110, ID_CHALLENGE = 0x10, ID_RESPONSE = 0x11, ID_VALID = 0x12};
    enum auth_type : uint8_t {AUTH_CHALLENGE_RESPONSE = 0};

    typedef struct Hello{
        const packet_id id = ID_HELLO;
        auth_type auth;
    } Hello;

    typedef struct Message{
        const packet_id id = ID_MESSAGE;
        uint8_t username[16];
        uint8_t signed_message[1024 + RSA_KEY_SIZE]; // this is a multiple of 16 yay
        uint16_t sz;
    } Message;

    typedef struct Challenge{
        const packet_id id = ID_CHALLENGE;
        uint8_t approved_sign[RSA_KEY_SIZE];
    } Challenge;

    typedef struct Response{
        const packet_id id = ID_RESPONSE;
        uint8_t approved_sign[RSA_KEY_SIZE];
        uint8_t pubkey[RSA_KEY_SIZE];
        uint8_t sign[RSA_KEY_SIZE];
        uint8_t message[1024];
    } Response;

    typedef struct Valid{
        const packet_id id = ID_VALID;
        uint8_t symkey[RSA_KEY_SIZE];
    } Valid;

    typedef struct Keys{
        const packet_id id = ID_KEYS;
        uint8_t mykey[RSA_KEY_SIZE + 16]; //Padding ??
    } Keys;

    union Generic_packet {Hello h; Message m; Challenge c; Response r; Valid v;};
};

#define MAX_PACKET_SIZE sizeof(Packets::Generic_packet)

#endif