#include "client.h"



namespace p2p_chat {

    using namespace global;



    Client::Client() {

        mPort = DEFAULT_CLIENT_PORT;

        mSaddrIn.sin_family = AF_INET;
        mSaddrIn.sin_port = htons(mPort);
        mSaddrIn.sin_addr.s_addr = INADDR_ANY;
        memset((&mSaddrIn.sin_zero), 0, 8);
    }



    int Client::startSession() {

        if (connect(mTcpSocket, reinterpret_cast<SOCKADDR*>(&mRemoteSaddrIn), sizeof(SOCKADDR_IN)) != 0) {
            logOnFailure("failed to connect");

            return -1;
        }
        else {
            recv(mTcpSocket, mRemoteUsername, MAX_USERNAME_LENGTH, 0);
            send(mTcpSocket, mUsername, MAX_USERNAME_LENGTH, 0);

            std::cout << "\nSuccessfuly connected to " << mRemoteUsername <<
                ".\n\n\n\nYou can start chatting (type \"\\EXIT\" to leave).\n" <<
                "Messages with no longer than 255 symbols are allowed.\n\n";

            return 0;
        }
    }



    void Client::setRemote(const char address[], const short& port) {

        mRemoteSaddrIn.sin_family = AF_INET;
        mRemoteSaddrIn.sin_port = htons(port);
        mRemoteSaddrIn.sin_addr.s_addr = inet_addr(address);
        memset(&(mRemoteSaddrIn.sin_zero), 0, 8);
    }

}
