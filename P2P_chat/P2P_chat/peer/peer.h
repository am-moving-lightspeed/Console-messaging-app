#pragma once

#include <winsock.h>
#include <iostream>
#include <thread>

#include "../global.h"



#ifndef P2P_CHAT_PEER
#define P2P_CHAT_PEER

namespace p2p_chat {

    using namespace global;



    class Peer {

        public:
            ~Peer();

            void setUsername(const char[]);
            virtual int startChat();
            virtual int initSession();

            virtual int startSession() = 0;


        protected:
            char mUsername[MAX_USERNAME_LENGTH];
            char mRemoteUsername[MAX_USERNAME_LENGTH];

            bool exitRequested = false;

            WSADATA mWsaData;

            SOCKADDR_IN mSaddrIn;
            SOCKADDR_IN mRemoteSaddrIn;
            
            //
            unsigned short mPort;

            SOCKET mTcpSocket;
            SOCKET mUdpSocket;

            inline void logOnSuccess(const char[]);
            inline void logOnFailure(const char[]);

            void receiveMessage();
            void sendMessage();

    };

}

#endif
