#pragma once



#ifndef P2P_CHAT_GLOBAL
#define P2P_CHAT_GLOBAL

namespace p2p_chat {

    namespace global {

        const unsigned short DEFAULT_BUFFER_SIZE = 256;
        const unsigned short DEFAULT_PAYLOAD_SIZE = DEFAULT_BUFFER_SIZE;
        const unsigned short DEFAULT_SERVER_PORT = 15785;
        const unsigned short DEFAULT_CLIENT_PORT = 15805;

        const unsigned short MAX_IPV4_LENGTH = 16;
        const unsigned short MAX_USERNAME_LENGTH = 32;

    }

}

#endif
