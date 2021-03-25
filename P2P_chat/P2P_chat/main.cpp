#define P2P_CHAT

#include <iostream>
#include <winsock.h>

#include "global.h"
#include "server.h"
#include "client.h"



void getUserChoice(char&);
void getServerAddress(char[], int&);



int main(int argc, char argv[]) {

    using namespace p2p_chat;
    using namespace p2p_chat::global;


    char userChoice;
    char kbInput[MAX_USERNAME_LENGTH];


    std::cout << "Choose option by inserting its number:\n1) Start session,\n2) Connect to session.\n> ";
    getUserChoice(userChoice);

    std::cout << "\nYour username (will be displayed to the one you gonna chat with): ";
    std::cin.getline(kbInput, MAX_USERNAME_LENGTH);


    switch (userChoice) {
        case '1':
        {
            Server server;

            server.setUsername(kbInput);
            int exitCode = server.initSession();
            if (exitCode != 0) {
                return exitCode;
            }

            exitCode = server.startSession();
            if (exitCode != 0) {
                return exitCode;
            }

            server.startChat();

            break;
        }

        case '2':
        {
            int port = -1;
            char ipv4[MAX_IPV4_LENGTH];
            getServerAddress(ipv4, port);

            Client client;

            client.setUsername(kbInput);
            int exitCode = client.initSession();
            if (exitCode != 0) {
                return exitCode;
            }

            client.setRemote(ipv4, port);
            exitCode = client.startSession();
            if (exitCode != 0) {
                return exitCode;
            }

            client.startChat();

            break;
        }

        default:
            break;
    }


    WSACleanup();
    return 0;
}



void getUserChoice(char& userChoice) {

    while (true) {
        std::cin.get(userChoice);

        if (userChoice != '1' && userChoice != '2') {

            std::cin.ignore(LLONG_MAX, '\n');
            std::cout << "Wrong value. Maybe different?\n> ";
        }
        else {
            std::cin.ignore(LLONG_MAX, '\n');
            break;
        }
    }
}



void getServerAddress(char ipv4[], int& port) {

    using namespace p2p_chat::global;


    std::cout << "Address (IPv4): ";
    std::cin.getline(ipv4, MAX_IPV4_LENGTH);

    std::cout << "Port: ";
    std::cin >> port;
    std::cin.ignore(LLONG_MAX, '\n');

    if (port < 1024 && port > sizeof(short)) {
        port = p2p_chat::global::DEFAULT_SERVER_PORT;
    }
}
