#define P2P_CHAT

#include <iostream>
#include <winsock.h>

#include "global.h"
#include "server.h"
#include "client.h"



char getUserChoice();
void getServerAddress(char[], int&);



int main(int argc, char argv[]) {

    using namespace p2p_chat;
    using namespace p2p_chat::global;


    char userChoice;
    char kbInput[DEFAULT_BUFFER_SIZE];


    std::cout << "Choose option by inserting its number:\n1) Start session,\n2) Connect to session.\n\n> ";
    userChoice = getUserChoice();


    switch (userChoice) {
        case '1':
        {
            Server server;

            int ret = server.initSession();
            if (ret != 0) {
                return ret;
            }

            ret = server.startSession();

            break;
        }

        case '2':
        {
            int port = -1;
            char ipv4[MAX_IPV4_LENGTH];
            getServerAddress(ipv4, port);

            Client client;
            break;
        }

        default:
            break;
    }






    /*while (true) {
        FD_ZERO(&fr);
        FD_ZERO(&fw);
        FD_ZERO(&fe);

        FD_SET(tcpSocket, &fr);
        FD_SET(tcpSocket, &fe);

        timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        ret = select(tcpSocket, &fr, &fw, &fe, &tv);

        if (ret < 0) {
            std::cout << "Available";
        }
        else if (ret == 0) {
            std::cout << "Unavailable";
        }
        else {
            std::cout << "Error";
        }
    }*/


    WSACleanup();
    return 0;
}



char getUserChoice() {

    char userChoice;

    while (true) {
        std::cin.get(userChoice);

        if (userChoice != '1' && userChoice != '2') {

            std::cin.ignore(LLONG_MAX, '\n');
            std::cout << "Wrong value. Maybe different\n> ";
        }
        else {
            break;
        }
    }

    return userChoice;
}



void getServerAddress(char ipv4[], int& port) {

    using namespace p2p_chat::global;


    std::cin.ignore(LLONG_MAX, '\n');
    std::cout << "Address (IPv4): ";
    std::cin.getline(ipv4, MAX_IPV4_LENGTH);

    std::cout << "Port: ";
    std::cin >> port;

    if (port < 1024 && port > sizeof(short)) {
        port = p2p_chat::global::DEFAULT_SERVER_PORT;
    }
}
