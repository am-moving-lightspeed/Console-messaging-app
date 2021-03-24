#include "client.h"



namespace p2p_chat {

    int Client::startSession() {

        if (listen(mTcpSocket, 1) != 0) {
            logOnFailure("failed starting listening to port");
            return WSAGetLastError();
        }
        else {
            logOnSuccess("started listening to incoming connection requests...");
        }

        return 0;
    }

}
