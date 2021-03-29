#include <iostream>
#include <winsock.h>

#include "global.h"
#include "peer.h"



int main(int argc, char argv[]) {

    using namespace p2p_chat;
    using namespace p2p_chat::global;


    char kbInput[MAX_USERNAME_LENGTH];

    std::cout << "Your username (will be displayed to the one you gonna chat with): ";
    std::cin.getline(kbInput, MAX_USERNAME_LENGTH);

    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(LLONG_MAX, '\0');
    }


    Peer* peer = new Peer();

    peer->setUsername(kbInput);

    int exitCode = peer->initSession();
    if (exitCode != 0) {
        return exitCode;
    }

    exitCode = peer->startSession();
    if (exitCode != 0) {
        return exitCode;
    }

    peer->startChat();


    delete peer;

    WSACleanup();
    return 0;
}
