#pragma once

#include <winsock.h>

#include "peer.h"



#ifndef P2P_CHAT_CLIENT
#define P2P_CHAT_CLIENT

namespace p2p_chat {

    class Client sealed : public Peer {

        public:
            Client();

            virtual int startSession() override;

            void setRemote(const char[], const short&);

    };

}

#endif
