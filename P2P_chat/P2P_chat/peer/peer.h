#pragma once

#include <winsock.h>
#include <iostream>

#include "../global.h"



#ifndef P2P_CHAT_PEER
#define P2P_CHAT_PEER

namespace p2p_chat {

    class Peer {

        public:
            Peer();
            ~Peer();

            virtual int initSession();
            virtual int startSession() = 0;


        protected:
            WSADATA mWsaData;
            SOCKADDR_IN mSaddrIn;
            unsigned short mPort;

            SOCKET mTcpSocket;
            SOCKET mUdpSocket;

            inline void logOnSuccess(const char[]);
            inline void logOnFailure(const char[]);
            virtual unsigned short tryBindToPort();

    };

}

#endif
