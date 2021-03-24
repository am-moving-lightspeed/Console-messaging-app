#include "peer.h"



namespace p2p_chat {

    using namespace global;



    Peer::Peer() {

        mPort = DEFAULT_SERVER_PORT;

        mSaddrIn.sin_family = AF_INET;
        mSaddrIn.sin_port = htons(mPort);
        mSaddrIn.sin_addr.s_addr = INADDR_ANY;
        memset((&mSaddrIn.sin_zero), 0, 8);
    }



    Peer::~Peer() {

        if (closesocket(mTcpSocket) != 0) {
            logOnFailure("failed to close TCP socket");
        }
        else {
            logOnSuccess("TCP socket's been successfully closed");
        }

        if (closesocket(mUdpSocket) != 0) {
            logOnFailure("failed to close UDP socket");
        }
        else {
            logOnSuccess("UDP socket's been successfully closed");
        }

        if (WSACleanup() != 0) {
            logOnFailure("failed to terminate Winsock dll usage");
        }
        else {
            logOnSuccess("Winsock dll's been terminated");
        }
    }



    inline void Peer::logOnSuccess(const char buf[]) {

        std::cout << "OK: " << buf << "." << std::endl;
    }



    inline void Peer::logOnFailure(const char buf[]) {

        std::cout << "ERROR: " << buf << "." << std::endl;

    }



    unsigned short Peer::tryBindToPort() {

        unsigned short port = mPort;

        int i = 0, ret;
        while (i < 10) {

            if ((ret = bind(mTcpSocket, reinterpret_cast<SOCKADDR*>(&mSaddrIn), sizeof(sockaddr))) != 0) {
                logOnFailure("socket's failed to bind.");
            }
            else {
                logOnSuccess("socket's been bound.");
                return port;
            }

            i++;
        }

        return -1;
    }



    int Peer::initSession() {

        if (WSAStartup(MAKEWORD(2, 2), &mWsaData) != 0) {

            logOnFailure("occured at WSAStartup() call");
            return WSAGetLastError();
        }
        else {
            logOnSuccess("WSAStartup() call");
        }


        mTcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (mTcpSocket < 0) {

            logOnFailure("failed to create socket");
            return WSAGetLastError();
        }
        else {
            logOnSuccess("socket's been created");
        }


        int optValue = 0;
        if (setsockopt(mTcpSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&optValue), sizeof(int)) != 0) {

            logOnFailure("failed to set up socket options (check \"error-log.txt\" for info)");
            return WSAGetLastError();
        }
        else {
            logOnSuccess("socket's options' been set");
        }

        mPort = tryBindToPort();
        if (mPort < 0) {
            return WSAGetLastError();
        }

        return 0;
    }

}
