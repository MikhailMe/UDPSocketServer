#include "client_functions.h"

int main() {
    int client_socket;
    flag = false;
    datagram_number = 0;
    char buffer[BUFFER_SIZE + 1];
    struct sockaddr_in server{}, from;
    socklen_t length = sizeof(server);

    if ((client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        std::cerr << "socket error" << std::endl;
        return SOCKET_ERROR;
    }

    memset((char *) &server, 0, sizeof(server));
    bzero(buffer, BUFFER_SIZE);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    if (inet_aton("127.0.0.1", &server.sin_addr) <= 0) {
        std::cerr << "inet_aton() error" << std::endl;
        return INET_ATON_ERROR;
    }

    // FIXME : пишем любой номер датаграмы (кроме 0) - для перемешивания, например 8
    std::string request = CHECK_CONNECTION;
    addDatagramNumber(request, datagram_number);  // datagram_number
    if (sendto(client_socket, request.c_str(), sizeof(request), 0, (struct sockaddr *) &server, length) < 0) {
        std::cerr << "sendto error" << std::endl;
        return SENDTO_ERROR;
    }

    std::cout << "sent request" << std::endl;

    if (recvfrom(client_socket, buffer, sizeof(buffer), 0, (struct sockaddr *) &server, &length) < 0) {
        std::cerr << "recvfrom error" << std::endl;
        return RECVFROM_ERROR;
    }

    // _MIXING_$5$0
    std::string response(buffer);
    unsigned long first_index_of_fake_num = response.find_first_of(SEPARATOR);
    unsigned long last_index_of_fake_num = response.find_last_of(SEPARATOR);
    int from_server = datagram_number;
    if (last_index_of_fake_num <= response.size() - 1)
        from_server = std::atoi(response.substr(first_index_of_fake_num + 1, last_index_of_fake_num).c_str());

    if (datagram_number != from_server) {
        std::cout << "_MIXING" << std::endl;
        std::cout << "Send the message again with the correct datagram number" << std::endl;

        unsigned long last_index_of_right_num = response.find_last_of(SEPARATOR);
        int right_number = std::atoi(response.substr(last_index_of_right_num  + 1, response.size() - 1).c_str());

        datagram_number = right_number;

        cutDatagramNumber(request);
        addDatagramNumber(request, datagram_number);
        if (sendto(client_socket, request.c_str(), sizeof(request), 0, (struct sockaddr *) &server, length) < 0) {
            std::cerr << "sendto error" << std::endl;
            return SENDTO_ERROR;
        }


        if (recvfrom(client_socket, buffer, sizeof(buffer), 0, (struct sockaddr *) &server, &length) < 0) {
            std::cerr << "recvfrom error" << std::endl;
            return RECVFROM_ERROR;
        }
        response = buffer;
    }
    if (response != SUCCESS_CONNECTION) {
        std::cerr << "connection not established" << std::endl;
        return NO_CONNECTION_ERROR;
    }

    auto &&client_thread = std::make_unique<std::thread>(client_read_handler, client_socket, from);

    cutDatagramNumber(buffer);
    std::cout << "datagram_num = " << datagram_number << " | message = " << buffer << "\n" << std::endl;
    datagram_number++;
    std::cout << "*********************************************************" << std::endl;
    while (true) {
        std::string command;
        std::getline(std::cin, command);
        if (command == _TEXT) {
            flag = false;
            text(client_socket, server, datagram_number);
        } else if (command == _COUNT) {
            flag = false;
            count(client_socket, server, datagram_number);
        } else if (command == _EXIT) {
            flag = false;
            if (sendto(client_socket, "", sizeof(""), 0, (struct sockaddr *) &server, length) < 0) {
                std::cerr << "sendto error" << std::endl;
                return SENDTO_ERROR;
            }
            shutdown(client_socket, SHUT_RDWR);
            close(client_socket);
            break;
        } else {
            flag = true;
            std::string message(command);
            std::cout << "datagram_num = " << datagram_number << " | message = \"" << message << "\"" << std::endl;
            addDatagramNumber(message, datagram_number);
            if (sendto(client_socket, message.c_str(), sizeof(message), 0, (struct sockaddr *) &server, length) < 0) {
                std::cerr << "sendto error" << std::endl;
                return SENDTO_ERROR;
            }
        }
    }
    client_thread->join();
    shutdown(client_socket, SHUT_RDWR);
    close(client_socket);
    std::cout << "Goodbye from client" << std::endl;
    return 0;
}