#pragma once

#include "constants.h"

bool contains(int index) {
    for (std::vector<client>::iterator it = clients.begin(); it != clients.end(); it++) {
        if (it->number_connection == index) {
            return true;
        }
    }
    return false;
}

void list() {
    std::cout << "The list of clients:" << std::endl;
    for (std::vector<client>::iterator it = clients.begin(); it != clients.end(); it++) {
        std::cout << it->number_connection << " : " << it->ip << ":" << it->port << std::endl;
    }
}

// FIXME
void write(int &index, int &socket) {
    client cl = clients[index];
    sockaddr_in from{};
    memset((char *) &from, 0, sizeof(from));
    from.sin_family = AF_INET;
    from.sin_port = htons(cl.port);
    from.sin_addr.s_addr = inet_addr(cl.ip);
    socklen_t from_size = sizeof(from);
    std::cout << "Enter message to client " << cl.ip << ":" << cl.port << " : ";


    std::string message;
    std::getline(std::cin, message);
    if (sendto(socket, message.c_str(), sizeof(message), 0, (sockaddr *) (&from), from_size) < 0) {
        std::cerr << "sendto error" << std::endl;
        return;
    }
}


// FIXME
void kill(int index) {
    std::cout << "in kill" << std::endl;
}

// FIXME
void killall() {
    std::cout << "in killall" << std::endl;
}

int whom() {
    int result;
    do {
        std::cout << "Enter client id: ";
        std::cin >> result;
    } while (!contains(result));
    return result;
}