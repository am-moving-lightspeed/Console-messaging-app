#include "peer.h"



namespace p2p_chat {

    using namespace global;



    Peer::~Peer() {

        if (closesocket(mTcpSocket) != 0) {
            logOnFailure("failed to close TCP socket");
        }
        else {
            std::cout << "\n\n";
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



    void Peer::setUsername(const char buf[]) {

        strcpy_s(mUsername, MAX_USERNAME_LENGTH, buf);
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
        mUdpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (mTcpSocket < 0 || mUdpSocket < 0) {

            logOnFailure("failed to create sockets");
            return WSAGetLastError();
        }
        else {
            logOnSuccess("sockets' been created");
        }


        BOOL false_ = 0;
        if (setsockopt(mTcpSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&false_), sizeof(BOOL)) != 0 ||
            bind(mTcpSocket, reinterpret_cast<SOCKADDR*>(&mSaddrIn), sizeof(SOCKADDR_IN)) != 0) {

            logOnFailure("failed to bind TCP socket");
            return WSAGetLastError();
        }
        else {
            logOnSuccess("TCP socket's been bound");
        }


        BOOL true_ = 1;
        u_long opt = 1;
        if (setsockopt(mUdpSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&true_), sizeof(BOOL)) != 0 ||
            ioctlsocket(mUdpSocket, FIONBIO, &opt) != 0 ||
            bind(mUdpSocket, reinterpret_cast<SOCKADDR*>(&mSaddrIn), sizeof(SOCKADDR_IN)) != 0) {

            logOnFailure("failed to bind UDP socket");
            std::cout << WSAGetLastError();
            return WSAGetLastError();
        }
        else {
            logOnSuccess("UDP socket's been bound");
        }


        return 0;
    }



    int Peer::startChat() {
        char message[DEFAULT_BUFFER_SIZE];
        std::thread* receiver = new std::thread(&Peer::receiveMessage, this);

        while (!exitRequested) {
            std::cout << "you> ";
            std::cin.getline(message, DEFAULT_PAYLOAD_SIZE);

            if (strcmp(message, "\\EXIT") == 0) {

                exitRequested = true;
                sendto(mUdpSocket,
                       "\\EXIT\0",
                       7,
                       0,
                       reinterpret_cast<SOCKADDR*>(&mRemoteSaddrIn),
                       sizeof(SOCKADDR_IN));
            }
            else {
                sendto(mUdpSocket,
                       message,
                       DEFAULT_PAYLOAD_SIZE,
                       0,
                       reinterpret_cast<SOCKADDR*>(&mRemoteSaddrIn),
                       sizeof(SOCKADDR_IN));
            }
        }

        receiver->join();

        delete receiver;
        return 0;
    }



    void Peer::receiveMessage() {

        char message[DEFAULT_BUFFER_SIZE];

        while (!exitRequested) {

            int attrlen = sizeof(SOCKADDR_IN);
            int ret = recvfrom(mUdpSocket,
                               message,
                               DEFAULT_PAYLOAD_SIZE,
                               0,
                               reinterpret_cast<SOCKADDR*>(&mRemoteSaddrIn),
                               &attrlen);

            if (ret > 0) {
                if (strcmp(message, "\\EXIT\0") == 0) {

                    std::cout << "\nYour companion has left the chat (press Enter).";
                    exitRequested = true;
                }
                else {
                    std::cout << "\n" << mRemoteUsername << "> " << message << "\nyou> ";
                }
            }
        }
    }

}
