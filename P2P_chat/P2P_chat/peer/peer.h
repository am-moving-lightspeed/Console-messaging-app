#pragma once

#include <winsock.h>
#include <iostream>
#include <thread>
#include <list>
#include <string>
#include <chrono>

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
            struct Message {

                unsigned long long id;
                char message[BUFFER_SIZE];
                char author[MAX_USERNAME_LENGTH];


                void toString(char[]);
            };


            char mUsername[MAX_USERNAME_LENGTH];
            char mRemoteUsername[MAX_USERNAME_LENGTH];

            bool mIsWrongOrder = false;
            std::list<Message*> mHistory;
            bool mIsListLocked = false;

            bool exitRequested = false;

            WSADATA mWsaData;

            SOCKADDR_IN mSaddrIn;
            SOCKADDR_IN mRemoteSaddrIn;
            
            unsigned short mPort;

            SOCKET mTcpSocket;
            SOCKET mUdpSocket;

            inline void logOnSuccess(const char[]);
            inline void logOnFailure(const char[]);

            void receiveMessage();
            void pushToHistory(const char[], const char[]);
            void appendId(const char[], char[]);
            void removeId(const char[], char[]);
            void resolveMessageOrder(Message* const);
            void redrawChat();

    };

}

#endif
