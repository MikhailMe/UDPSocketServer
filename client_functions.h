#pragma once

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iomanip>

// error codes
#define SOCKET_ERROR        (-1)
#define SENDTO_ERROR        (-2)
#define RECVFROM_ERROR      (-3)
#define INET_ATON_ERROR     (-4)
#define NO_CONNECTION_ERROR (-5)

// operations
#define _SQRT              "sqrt"
#define _FACTORIAL          "fact"

// commands
#define _TEXT              "text"
#define _COUNT             "count"
#define _EXIT              "shutdown"

// separator
#define SEPARATOR          "$"
#define SPEC_SYMB          '@'

// messages
#define MIXING             "MIXING"
#define DUPLICATED         "_DUPLICATED_"
#define CHECK_CONNECTION   "_check_connection_"
#define SUCCESS_CONNECTION "_success_connection_"

const size_t BUFFER_SIZE = 512;
const size_t PORT = 7777;
int datagram_number;
static bool flag;


void addDatagramNumber(std::string &message, int number) {
    message.append(SEPARATOR);
    message.append(std::to_string(number));
}

// message$datagram_number
int getDatagamNumber(std::string &message) {
    unsigned long start_index_of_num = message.find_first_of(SEPARATOR) + 1;
    size_t end_index_of_num = message.size();
    std::string number = message.substr(start_index_of_num, end_index_of_num);
    message = message.substr(0, start_index_of_num - 1);
    return std::atoi(number.c_str());
}

std::string cutDatagramNumber(std::string &stringWithDatagramNumber) {
    unsigned long start_index_of_num = stringWithDatagramNumber.find_first_of(SEPARATOR);
    stringWithDatagramNumber = stringWithDatagramNumber.substr(0, start_index_of_num);
    return stringWithDatagramNumber;
}

std::string cutDatagramNumber(char charArrayWithDatagramNumber[]) {
    std::string stringWithDatagramNumber(charArrayWithDatagramNumber);
    unsigned long start_index_of_num = stringWithDatagramNumber.find_first_of(SEPARATOR);
    stringWithDatagramNumber = stringWithDatagramNumber.substr(0, start_index_of_num);
    return stringWithDatagramNumber;
}

bool hasSpecSymb(char buffer[]) {
    return buffer[0] == (char) SPEC_SYMB ;
}

void client_read_handler(int client_socket, sockaddr_in from) {
    char buffer[BUFFER_SIZE + 1];
    socklen_t length = sizeof(from);
    int counter = 0;
    while (true) {

        /*
         *  НАЧАЛО ОБРАБОТКИ ПОТЕРИ ДЕЙТАГРАММЫ
         *
         * */

        struct timeval timeout = {3, 0};
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(client_socket, &readSet);

        auto &&select_status = select(client_socket + 1, &readSet, NULL, NULL, &timeout);

        if (counter == 3) {
            datagram_number--;
            break;
        }

        if (select_status > 0) {

            if (FD_ISSET(client_socket, &readSet)) {
                if (recvfrom(client_socket, buffer, sizeof(buffer), 0, (struct sockaddr *) &from, &length) < 0) {
                    std::cerr << "recvfrom error" << std::endl;
                    break;
                }
                flag = false;
                if (hasSpecSymb(buffer)){
                    datagram_number++;
                }
            } else {
                continue;
            }
        } else if (select_status == 0) {
            if (flag) {
                counter++;
                std::cout << "send one more" << std::endl;
                std::string one_more("lost?");
                addDatagramNumber(one_more, datagram_number);
                if (sendto(client_socket, one_more.c_str(), sizeof(one_more), 0, (struct sockaddr *) &from, length) < 0) {
                    std::cerr << "sendto error" << std::endl;
                    break;
                }
            }
            continue;
        } else {
            std::cerr << "select error" << std::endl;
            return;
        }

        /*
       *  КОНЕЦ ОБРАБОТКИ ПОТЕРИ ДЕЙТАГРАММЫ
       *
       *
       * */

        // string = message$number$DUPLICATED
        // parsing
        std::string response(buffer);
        unsigned long start_index_of_num = response.find_first_of(SEPARATOR);
        std::string repeate_send_message = response.substr(0, start_index_of_num);
        unsigned long last_index_of_num = response.find_last_of(SEPARATOR);
        int number = std::atoi(response.substr(start_index_of_num + 1, last_index_of_num).c_str());
        unsigned long last_index_of_message = response.size() - 1;
        response = response.substr(last_index_of_num + 1, last_index_of_message);

        if (strcmp(buffer, _EXIT) == 0) {
            break;
        }
        if (response == DUPLICATED) {
            std::cout << DUPLICATED << std::endl;
            std::cout << "This problem is solved!" << std::endl;
            number++;
            addDatagramNumber(repeate_send_message, number);
            if (sendto(client_socket, repeate_send_message.c_str(), sizeof(repeate_send_message), 0,
                       reinterpret_cast<const sockaddr *>(&from), length) < 0) {
                std::cerr << "sendto error" << std::endl;
                return;
            }
            datagram_number = number;
            cutDatagramNumber(repeate_send_message);
            std::cout << "datagram_num = " << number << " | message = " << repeate_send_message << "\n" << std::endl;
            datagram_number++;
            continue;
        }
        if (response == MIXING) {
            std::cout << MIXING << std::endl;
            std::cout << "This problem is solved!" << std::endl;
            continue;
        }

        // @message$datagram_number
        if (hasSpecSymb(buffer)) {
            std::string resp(buffer);
            resp = resp.substr(1, resp.size());
            cutDatagramNumber(resp);
            std::strcpy(buffer, resp.c_str());
        }
        std::cout << "Server's response: \"" << buffer << "\"" << std::endl;
        std::cout << "*********************************************************" << std::endl;
    }
    std::cout << "Server is unreachable" << std::endl;
}

void text(int client_socket, sockaddr_in server, int &datagram_number) {
    socklen_t server_size = sizeof(server);
    std::string command = _TEXT;
    addDatagramNumber(command, datagram_number);
    // отправили серверу уведомление, что сейчас будет отправлен текст
    if (sendto(client_socket, command.c_str(), sizeof(command), 0, reinterpret_cast<const sockaddr *>(&server), server_size) < 0) {
        std::cerr << "sendto error" << std::endl;
        return;
    }
    cutDatagramNumber(command);
    std::cout << "datagram_num = " << datagram_number << " | message = " << command << "\n" << std::endl;
    // FIXME закоментировать для "дублирования"
    datagram_number++;
    std::string message;
    std::cout << "Your message to server: ";
    std::getline(std::cin, message);
    addDatagramNumber(message, datagram_number);
    if (sendto(client_socket, message.c_str(), sizeof(message), 0, reinterpret_cast<const sockaddr *>(&server),
               server_size) < 0) {
        std::cerr << "sendto error" << std::endl;
        return;
    }
    cutDatagramNumber(message);
    std::cout << "datagram_num = " << datagram_number << " | message = " << message << "\n" << std::endl;
    datagram_number++;
    std::cout << "message: \"" << message << "\" send to server" << std::endl;
}

void count(int client_socket, sockaddr_in server, int &datagram_number) {
    std::string count = _COUNT;
    socklen_t server_size = sizeof(server);
    addDatagramNumber(count, datagram_number);
    // отправили серверу уведомление, что сейчас будет отправлен математическое выражение
    if (sendto(client_socket, count.c_str(), sizeof(count), 0, reinterpret_cast<const sockaddr *>(&server),
               server_size) < 0) {
        std::cerr << "sendto error" << std::endl;
        return;
    }
    cutDatagramNumber(count);
    std::cout << "datagram_num = " << datagram_number << " | message = " << count << "\n" << std::endl;
    datagram_number++;

    std::string computation;
    std::string first_num;
    std::cout << "Enter first number: ";
    std::getline(std::cin, first_num);
    addDatagramNumber(first_num, datagram_number);
    if (sendto(client_socket, first_num.c_str(), sizeof(first_num), 0, reinterpret_cast<const sockaddr *>(&server),
               server_size) < 0) {
        std::cerr << "sendto error" << std::endl;
        return;
    }
    cutDatagramNumber(first_num);
    std::cout << "datagram_num = " << datagram_number << " | first number = " << first_num << "\n" << std::endl;
    datagram_number++;

    std::string operation;
    std::cout << "Enter operation: ";
    std::getline(std::cin, operation);
    addDatagramNumber(operation, datagram_number);
    if (sendto(client_socket, operation.c_str(), sizeof(operation), 0, reinterpret_cast<const sockaddr *>(&server),
               server_size) < 0) {
        std::cerr << "sendto error" << std::endl;
        return;
    }
    cutDatagramNumber(operation);
    std::cout << "datagram_num = " << datagram_number << " | operation = " << operation << "\n" << std::endl;
    datagram_number++;
    // если выполняем долгую операцию => вторая чиселка просто не нужна!
    if (strcmp(operation.c_str(), _SQRT) != 0 && strcmp(operation.c_str(), _FACTORIAL) != 0) {
        computation = first_num + " " + operation + " ";
        // вторая чиселка
        std::string second_num;
        std::cout << "Enter second number: ";
        std::getline(std::cin, second_num);
        addDatagramNumber(second_num, datagram_number);
        if (sendto(client_socket, second_num.c_str(), sizeof(second_num), 0,
                   reinterpret_cast<const sockaddr *>(&server), server_size) < 0) {
            std::cerr << "sendto error" << std::endl;
            return;
        }
        cutDatagramNumber(second_num);
        std::cout << "datagram_num = " << datagram_number << " | second number = " << second_num << "\n"
                  << std::endl;
        datagram_number++;
        computation += second_num;
    } else {
        computation = operation + "(" + first_num + ")";
    }
    std::cout << "Client's request: " << computation << std::endl;
}