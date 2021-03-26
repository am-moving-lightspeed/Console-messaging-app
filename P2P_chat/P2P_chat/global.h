#pragma once

#include <Windows.h>



#ifndef P2P_CHAT_GLOBAL
#define P2P_CHAT_GLOBAL

namespace p2p_chat {

    namespace global {

        const unsigned short BUFFER_SIZE = 256;
        const unsigned short ID_SIZE = 20;
        const unsigned short BUFFER_SIZE_WITH_ID = BUFFER_SIZE + ID_SIZE;

        const unsigned short DEFAULT_SERVER_PORT = 15785;
        const unsigned short DEFAULT_CLIENT_PORT = 15805;

        const unsigned short MAX_IPV4_LENGTH = 16;
        const unsigned short MAX_USERNAME_LENGTH = 32;

        const char DEFAULT_CHAT_HEADER[] =
            "You can write messages now (type \"\\EXIT\" to leave).\n"
            "Messages with no longer than 256 symbols are allowed.\n\n";


        void clearScreen();

    }

}

#endif
