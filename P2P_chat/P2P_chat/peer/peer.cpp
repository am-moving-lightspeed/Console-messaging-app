#include "peer.h"



namespace p2p_chat {

    using namespace global;



    Peer::Peer() {

        mPort = DEFAULT_PORT;

        mSaddrIn.sin_family = AF_INET;
        mSaddrIn.sin_port = htons(mPort);
        mSaddrIn.sin_addr.s_addr = INADDR_ANY;
        memset(&(mSaddrIn.sin_zero), 0, 8);
    }



    Peer::~Peer() {

        if (closesocket(mTcpSocketListener) != 0) {
            logOnFailure("failed to close TCP l-socket");
        }
        else if (closesocket(mTcpSocketConnector) != 0) {
            logOnFailure("failed to close TCP c-socket");
        }
        else if (closesocket(mUdpSocket) != 0) {
            logOnFailure("failed to close UDP socket");
        }
        else {
            std::cout << "\n\n";
            logOnSuccess("Sockets' been successfully closed");
        }

        if (WSACleanup() != 0) {
            logOnFailure("failed to terminate Winsock dll usage");
        }
        else {
            logOnSuccess("Winsock dll's been terminated");
        }


        for (auto i = mHistory.begin(); i != mHistory.end(); i++) {
            delete *i;
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

        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {

            logOnFailure("occured at WSAStartup() call");
            return WSAGetLastError();
        }
        else {
            logOnSuccess("WSAStartup() call");
        }


        mTcpSocketListener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        mTcpSocketConnector = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        mUdpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (mTcpSocketListener < 0 || mTcpSocketConnector < 0 || mUdpSocket < 0) {

            logOnFailure("failed to create sockets");
            return WSAGetLastError();
        }
        else {
            logOnSuccess("sockets' been created");
        }


        BOOL false_ = 0;
        BOOL true_ = 1;
        u_long opt = 1;
        if (setsockopt(mTcpSocketListener, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&false_), sizeof(BOOL)) != 0 ||
            setsockopt(mTcpSocketConnector, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&false_), sizeof(BOOL)) != 0 ||
            setsockopt(mUdpSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&true_), sizeof(BOOL)) != 0 ||
            ioctlsocket(mUdpSocket, FIONBIO, &opt) != 0) {

            logOnFailure("failed to set sockets' options");
            return WSAGetLastError();
        }
        else {
            logOnSuccess("sockets' options' been set");
        }


        return tryBindSockets();
    }



    int Peer::tryBindSockets() {

        if (tryBindTcpSocket(mTcpSocketListener, mPort) != 0) {

            logOnFailure("failed to bind TCP l-socket");
            return WSAGetLastError();
        }
        else {
            logOnSuccess("TCP l-socket's been bound");
        }

        u_short tmp = mPort + 1;
        if (tryBindTcpSocket(mTcpSocketConnector, tmp) != 0) {

            logOnFailure("failed to bind TCP c-socket");
            return WSAGetLastError();
        }
        else {
            logOnSuccess("TCP c-socket's been bound");
        }


        if (bind(mUdpSocket, reinterpret_cast<SOCKADDR*>(&mSaddrIn), sizeof(SOCKADDR_IN)) != 0) {

            logOnFailure("failed to bind UDP socket");
            return WSAGetLastError();
        }
        else {
            logOnSuccess("UDP socket's been bound");
            std::cout << "(port " << mPort << ")\n";
        }

        return 0;
    }



    int Peer::tryBindTcpSocket(const SOCKET& sock, unsigned short& port) {

        while (port < USHRT_MAX - 1) {

            if (bind(sock, reinterpret_cast<SOCKADDR*>(&mSaddrIn), sizeof(SOCKADDR_IN)) != 0) {

                if (WSAGetLastError() != WSAEADDRINUSE) {
                    return WSAGetLastError();
                }

                port += 1;
                mSaddrIn.sin_port = htons(port);
            }
            else {
                break;
            }
        }

        return 0;
    }



    int Peer::startSession() {

        if (listen(mTcpSocketListener, 1) != 0) {

            logOnFailure("failed starting listening to port");
            return WSAGetLastError();
        }
        else {
            logOnSuccess("started listening to incoming connection requests..");
        }


        getRemoteAddress();

        std::thread* acceptor = new std::thread(&Peer::startAcceptingConnectionRequests, this);

        int count = 0;
        while (true) {
            if (connect(mTcpSocketConnector, reinterpret_cast<SOCKADDR*>(&mRemoteSaddrIn), sizeof(SOCKADDR_IN)) != 0) {

                if (WSAGetLastError() != WSAECONNREFUSED) {

                    logOnFailure("failed to connect");
                    return WSAGetLastError();
                }
            }
            else {
                send(mTcpSocketConnector, mUsername, MAX_USERNAME_LENGTH, 0);
                break;
            }

            if (count % 1 == 0) {
                std::cout << "\nStill trying to connect.";
            }
            count++;

            Sleep(5000);
        }


        acceptor->join();

        delete acceptor;
        return 0;
    }



    void Peer::getRemoteAddress() {

        using namespace p2p_chat::global;


        char ipv4[MAX_IPV4_LENGTH];
        std::cout << "\nEnter remote address\nAddress (IPv4): ";
        std::cin.getline(ipv4, MAX_IPV4_LENGTH);

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(LLONG_MAX, '\0');
        }


        unsigned short port;
        std::cout << "Port: ";
        std::cin >> port;
        std::cin.ignore(LLONG_MAX, '\n');

        if (port <= 1024 && port > USHRT_MAX - 1) {
            port = DEFAULT_PORT;
        }

        while (true) {
            if (!mRemoteSaddrMutex) {
                mRemoteSaddrMutex = true;

                mRemoteSaddrIn.sin_family = AF_INET;
                mRemoteSaddrIn.sin_addr.s_addr = inet_addr(ipv4);
                mRemoteSaddrIn.sin_port = htons(port);
                memset(&(mRemoteSaddrIn.sin_zero), 0, 8);

                mRemoteSaddrMutex = false;
                break;
            }

            Sleep(500);
        }
    }



    void Peer::startAcceptingConnectionRequests() {

        FD_SET fr;
        TIMEVAL tv;

        while (!mExitRequested) {

            FD_ZERO(&fr);
            FD_SET(mTcpSocketListener, &fr);

            tv.tv_sec = 2;
            tv.tv_usec = 0;

            int ret = select(mTcpSocketListener + 1, &fr, nullptr, nullptr, &tv);
            if (ret > 0) {

                std::cout << "\nProcessing incoming connection...";
                acceptIncomingConnectionRequest();
                break;
            }
        }
    }



    void Peer::acceptIncomingConnectionRequest() {

        int addrlen = sizeof(SOCKADDR_IN);

        while (true) {

            if (!mRemoteSaddrMutex) {
                mRemoteSaddrMutex = true;

                SOCKET socket = accept(mTcpSocketListener, reinterpret_cast<SOCKADDR*>(&mRemoteSaddrIn), &addrlen);

                if (socket < 0) {
                    logOnFailure("failed to accept incoming connection");
                }
                else {
                    recv(socket, mRemoteUsername, MAX_USERNAME_LENGTH, 0);

                    std::cout << "\n" << mRemoteUsername << " has successfuly connected to you.";
                }

                mRemoteSaddrMutex = false;
                break;
            }

            Sleep(500);
        }
    }



    int Peer::startChat() {

        char message[BUFFER_SIZE];
        char out[BUFFER_SIZE_WITH_ID];

        Sleep(1000);
        redrawChat();

        std::thread* receiver = new std::thread(&Peer::receiveMessage, this);

        while (!mExitRequested) {
            std::cin.getline(message, BUFFER_SIZE);

            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(LLONG_MAX, '\n');
            }

            if (strcmp(message, "\\EXIT") == 0) {

                mExitRequested = true;
                sendto(mUdpSocket,
                       "\\EXIT\0",
                       7,
                       0,
                       reinterpret_cast<SOCKADDR*>(&mRemoteSaddrIn),
                       sizeof(SOCKADDR_IN));
            }
            else if (message[0] == '\0') {
                redrawChat();
            }
            else {
                appendId(message, out);
                pushToHistory(out, "you");
                redrawChat();

                sendto(mUdpSocket,
                       out,
                       BUFFER_SIZE_WITH_ID,
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

        char message[BUFFER_SIZE_WITH_ID];
        char in[BUFFER_SIZE];

        while (!mExitRequested) {

            int attrlen = sizeof(SOCKADDR_IN);
            int ret = recvfrom(mUdpSocket,
                               message,
                               BUFFER_SIZE_WITH_ID,
                               0,
                               reinterpret_cast<SOCKADDR*>(&mRemoteSaddrIn),
                               &attrlen);

            if (ret > 0) {
                if (strcmp(message, "\\EXIT\0") == 0) {

                    std::cout << "\nYour companion has left the chat (press Enter).";
                    mExitRequested = true;
                }
                else {
                    removeId(message, in);
                    pushToHistory(message, mRemoteUsername);
                    redrawChat();
                }
            }

            Sleep(200);
        }
    }



    void Peer::pushToHistory(const char message[], const char author[]) {

        while (true) {

            if (!mListMutex) {
                mListMutex = true;

                std::string s(message);

                char id[ID_SIZE + 1];
                memcpy_s(id, ID_SIZE, message, ID_SIZE);
                id[ID_SIZE] = '\0';

                Message* msg = new Message();
                msg->id = std::stoll(id);
                memcpy_s(msg->message, BUFFER_SIZE, s.substr(ID_SIZE).c_str(), BUFFER_SIZE);
                memcpy_s(msg->author, MAX_USERNAME_LENGTH, author, MAX_USERNAME_LENGTH);

                resolveMessageOrder(msg);

                mListMutex = false;
                break;
            }

            Sleep(50);
        }

    }



    void Peer::appendId(const char message[], char out[]) {

        std::string s = std::to_string(
            std::chrono::system_clock::now().time_since_epoch().count()
        );

        s.insert(0, ID_SIZE - s.size(), '0');

        s.append(message);

        memcpy_s(out, BUFFER_SIZE_WITH_ID, s.c_str(), BUFFER_SIZE_WITH_ID);

    }



    void Peer::removeId(const char message[], char out[]) {

        std::string s(message);
        memcpy_s(out, BUFFER_SIZE, s.substr(ID_SIZE).c_str(), BUFFER_SIZE);
    }



    void Peer::resolveMessageOrder(Message* const msg) {

        if (mHistory.size() > 0) {
            for (auto i = mHistory.begin(), j = ++mHistory.begin(); j != mHistory.end(); i++, j++) {

                if (msg->id < (*j)->id) {

                    mHistory.insert(i, msg);
                    return;
                }
            }
        }

        mHistory.push_back(msg);
    }



    void Peer::Message::toString(char buf[]) {

        unsigned int size = BUFFER_SIZE + MAX_USERNAME_LENGTH + 3;

        std::string s(author);
        s.append("> ").append(message);

        memcpy_s(buf, size, s.c_str(), s.size() + 1);
    }



    void Peer::redrawChat() {

        char buf[BUFFER_SIZE + MAX_USERNAME_LENGTH + 3];

        clearScreen();
        std::cout << DEFAULT_CHAT_HEADER;

        for (auto i = mHistory.begin(); i != mHistory.end(); i++) {

            (*i)->toString(buf);
            std::cout << buf << std::endl;
        }

        std::cout << "you> ";
    }

}
