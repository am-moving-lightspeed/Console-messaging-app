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

        char message[BUFFER_SIZE];
        char out[BUFFER_SIZE_WITH_ID];

        std::thread* receiver = new std::thread(&Peer::receiveMessage, this);

        std::cout << "you> ";
        while (!exitRequested) {
            std::cin.getline(message, BUFFER_SIZE);

            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(LLONG_MAX, '\n');
            }

            if (strcmp(message, "\\EXIT") == 0) {

                exitRequested = true;
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

        while (!exitRequested) {

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
                    exitRequested = true;
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
            if (!mIsListLocked) {

                mIsListLocked = true;

                std::string s(message);

                char id[ID_SIZE + 1];
                memcpy_s(id, ID_SIZE, message, ID_SIZE);
                id[ID_SIZE] = '\0';

                Message* msg = new Message();
                msg->id = std::stoll(id);
                memcpy_s(msg->message, BUFFER_SIZE, s.substr(ID_SIZE).c_str(), BUFFER_SIZE);
                memcpy_s(msg->author, MAX_USERNAME_LENGTH, author, MAX_USERNAME_LENGTH);

                resolveMessageOrder(msg);

                mIsListLocked = false;
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

                    mIsWrongOrder = true;
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
