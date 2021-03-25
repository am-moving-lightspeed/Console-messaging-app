#include "server.h"



namespace p2p_chat {

    using namespace global;



    Server::Server() {

        mPort = DEFAULT_SERVER_PORT;

        mSaddrIn.sin_family = AF_INET;
        mSaddrIn.sin_port = htons(mPort);
        mSaddrIn.sin_addr.s_addr = INADDR_ANY;
        memset(&(mSaddrIn.sin_zero), 0, 8);
    }



    int Server::startSession() {

        if (listen(mTcpSocket, 1) != 0) {
            logOnFailure("failed starting listening to port");
            return WSAGetLastError();
        }
        else {
            logOnSuccess("started listening to incoming connection requests..");
            std::cout << "(Type \"\\EXIT\" to abort).";
        }

        std::thread* acceptor = new std::thread(&Server::startAcceptingConnectionRequests, this);
        acceptor->join();

        delete acceptor;
        return 0;
    }



    void Server::startAcceptingConnectionRequests() {

        FD_SET fr;
        TIMEVAL tv;
        int count = 0;

        while (!exitRequested) {
            FD_ZERO(&fr);
            FD_SET(mTcpSocket, &fr);

            tv.tv_sec = 2;
            tv.tv_usec = 0;

            if (select(mTcpSocket + 1, &fr, nullptr, nullptr, &tv) > 0) {

                std::cout << "\nProcessing incoming connection...";
                acceptIncomingConnectionRequest();
                break;
            }

            count++;
            if (count % 10 == 0) {
                std::cout << "\nStill looking for incoming requests (" << count * 10 << "sec).";
            }
        }
    }



    void Server::acceptIncomingConnectionRequest() {

        int addrlen = sizeof(SOCKADDR_IN);

        mClientSocketForTcpConn = accept(mTcpSocket, reinterpret_cast<SOCKADDR*>(&mRemoteSaddrIn), &addrlen);

        if (mClientSocketForTcpConn < 0) {
            logOnFailure("failed to accept incoming connection");
        }
        else {
            send(mClientSocketForTcpConn, mUsername, MAX_USERNAME_LENGTH, 0);
            recv(mClientSocketForTcpConn, mRemoteUsername, MAX_USERNAME_LENGTH, 0);

            std::cout << "\n\n" << mRemoteUsername << " has successfuly connected to you.\n\n\n\n" <<
                "You can start chatting (type \"\\EXIT\" to leave).\n" <<
                "Messages with no longer than 255 symbols are allowed.\n\n";
        }
    }

}
