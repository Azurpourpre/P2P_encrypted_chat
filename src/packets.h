#ifndef _PACKETS
#define _PACKETS

#include <cstdint>

/*
    Packet ID :
    0b0 -> Don't assign
    0b1 -> Message
    0b1? -> Authentication with known public key
        0x10 -> Challenge
        0x11 -> Response
    0b1?? -> Control Messages
        0b100 -> Hello
        0b101 -> Add public key Request
*/

namespace Packets{
    enum packet_id : uint8_t {HELLO = 0b100, MESSAGE = 0b1, CHALLENGE = 0b10, RESPONSE = 0b11, ADD_KEY_REQUEST = 0b101};
    enum auth_type : uint8_t {CHALLENGE_RESPONSE = 0};

    typedef struct Hello{
        const packet_id id = HELLO;
        auth_type auth;
    } Hello;

    typedef struct Message{
        const packet_id id = MESSAGE;
        char username[15];
        char message[1024];
    } Message;

    typedef struct Challenge{
        const packet_id id = CHALLENGE;
        char random[128];
        int n_zero;
    } Challenge;

    typedef struct Response{
        const packet_id id = RESPONSE;
        char random[128];
        char hash[56];
    } Response;

    typedef struct Add_key_request{
        const packet_id id = ADD_KEY_REQUEST;
        char pubkey[440];
        char message[1024];
    } Add_key_request;

    union Generic_packet {Hello h; Message m; Challenge c; Response r; Add_key_request akr;};
};

#define MAX_PACKET_SIZE sizeof(Packets::Generic_packet)

#endif