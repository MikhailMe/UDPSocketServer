#pragma once

#include <mutex>
#include <cstdint>
#include <vector>
#include <list>

// error codes
#define SOCKET_ERROR        (-1)
#define BIND_ERROR          (-2)
#define SENDTO_ERROR        (-3)
#define RECVFROM_ERROR      (-4)
#define NO_CONNECTION_ERROR (-5)

// operations
#define _PLUS         "+"
#define _MINUS        "-"
#define _MULT         "*"
#define _DIVIDE       "/"
#define _SQRT         "sqrt"
#define _FACTORIAL    "fact"

// client commands
#define _TEXT              "text"
#define _COUNT             "count"

// server commands
#define _LIST              "list"
#define _KILL              "kill"
#define _WRITE             "write"
#define _KILLALL           "killall"
#define _EXIT              "shutdown"

// seperator
#define SEPARATOR          "$"
#define SPEC_SYMB          "@"

// messages
#define MIXING             "_MIXING_"
#define DUPLICATED         "_DUPLICATED_"
#define CHECK_CONNECTION   "_check_connection_"
#define SUCCESS_CONNECTION "_success_connection_"

const size_t PORT = 7777;
const size_t TIMEOUT_SEC = 0;
const size_t BUFFER_SIZE = 512;
const size_t TIMEOUT_USEC = 100;

struct client {
    char *ip;
    uint16_t port;
    sockaddr_in from;
    // FIXME to add in list
    int datagram_amount;
    int number_connection;

    bool operator==(const client &data) {
        return (this->ip == data.ip && this->port == data.port);
    }
};

struct count_request {
    int socket;
    double first_number;
    double second_number;
    std::string operation;
    bool isSlow;
};

std::mutex cl_mutex;
std::mutex count_mutex;

std::vector<client> clients;


// FIXME потеря (select)

// дублирование (посылаем 1,1,1,1) - готово

// перемешивание (посылаем сразу 8) - готово