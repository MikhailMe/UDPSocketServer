#include <netinet/in.h>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <bits/unique_ptr.h>
#include <thread>
#include <mutex>

#include "constants.h"
#include "ThreadPool.h"
#include "slow_operations.h"
#include "server_commands.h"
#include "server_functions.h"

int main() {
    int server_socket;
    int datagram_number = 0;
    struct sockaddr_in server{};
    if ((server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("Could not create socket");
        return SOCKET_ERROR;
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    int yes = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_RCVBUF, &yes, sizeof(yes));
    if (bind(server_socket, reinterpret_cast<const sockaddr *>(&server), sizeof(struct sockaddr)) < 0) {
        std::cerr << "bind error" << std::endl;
        return BIND_ERROR;
    }
    std::cout << "Server started on port " << PORT << std::endl;
    std::cout << "Waiting for incoming connections..." << std::endl;
    auto &&thread_pool = std::make_shared<ThreadPool>(4);
    auto &&server_thread = std::make_unique<std::thread>(server_handler, server_socket, datagram_number, thread_pool);
    while (true) {
        std::string command;
        std::getline(std::cin, command);
        if (command == _LIST) {
            list();
        } else if (command == _WRITE) {
            int client_id = whom();
            write(client_id, server_socket);
        } else if (command == _KILL) {
            int client_id = whom();
            kill(client_id);
        } else if (command == _KILLALL) {
            killall();
        } else if (command == _EXIT) {
            std::cout << "client was closed" << std::endl;
            break;
        }
    }
    server_thread->join();
    shutdown(server_socket, SHUT_RDWR);
    close(server_socket);
    std::cout << "Goodbye" << std::endl;;
    return 0;
}
