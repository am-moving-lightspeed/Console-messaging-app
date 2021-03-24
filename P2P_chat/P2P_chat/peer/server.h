#pragma once

#include <winsock.h>

#include "peer.h"



#ifndef P2P_CHAT_SERVER
#define P2P_CHAT_SERVER

namespace p2p_chat {

    class Server sealed : public Peer {

        public:
            virtual int startSession() override;

    };
}

#endif
