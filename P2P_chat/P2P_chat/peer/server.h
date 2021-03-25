#pragma once

#include <winsock.h>
#include <thread>

#include "peer.h"



#ifndef P2P_CHAT_SERVER
#define P2P_CHAT_SERVER

namespace p2p_chat {

    using namespace global;



    class Server sealed : public Peer {

        public:
            Server();

            virtual int startSession() override;


        protected:
            SOCKET mClientSocketForTcpConn;

            void startAcceptingConnectionRequests();
            void acceptIncomingConnectionRequest();

    };
}

#endif
