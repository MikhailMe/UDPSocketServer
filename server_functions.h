#pragma once

#include "constants.h"

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

bool contains(client new_client) {
    for (std::vector<client>::iterator it = clients.begin(); it != clients.end(); it++) {
        if (it->operator==(new_client)) {
            return true;
        }
    }
    return false;
}

void text(int server_socket, client cl, sockaddr_in from, int &datagram_number) {
    bool is_duplicated;
    std::string text = _TEXT;
    char message[BUFFER_SIZE];
    bzero(message, BUFFER_SIZE);
    socklen_t from_size = sizeof(from);
    if (recvfrom(server_socket, message, sizeof(message), 0, (struct sockaddr *) &from, &from_size) < 0) {
        std::cerr << "recvfrom error" << std::endl;
        return;
    }
    std::string mes(message);
    int dg_num = getDatagamNumber(mes);
    std::cout << "что приняли  : " << dg_num << std::endl;
    std::cout << "что надо было: " << datagram_number << std::endl;

    if (dg_num == datagram_number - 1) {
        // message$number$DUPLICATED
        std::cout << DUPLICATED << std::endl;
        std::string dupl(mes);
        addDatagramNumber(dupl, dg_num);
        dupl.append(SEPARATOR);
        dupl.append(DUPLICATED);
        if (sendto(server_socket, dupl.c_str(), BUFFER_SIZE, 0, (sockaddr *) (&from), from_size) < 0) {
            std::cerr << "sendto error" << std::endl;
            return;
        }
        is_duplicated = true;
    } else {
        is_duplicated = false;
    }

    if (!is_duplicated) {
        std::cout << "########## datagram_num = " << dg_num << " | message = " << mes << "\n" << std::endl;
        std::cout << cl.ip << ":" << cl.port << " send: " << mes << std::endl;
        if (sendto(server_socket, mes.c_str(), BUFFER_SIZE, 0, (sockaddr *) (&from), from_size) < 0) {
            std::cerr << "sendto error" << std::endl;
            return;
        }
        datagram_number++;
        std::cout << "message: \"" << mes << "\" send to " << cl.ip << ":" << cl.port << std::endl;
    }
}

// обработка входного примера
double operation_processing(double a, const std::string &operation, double b) {
    double result = 0;
    if (operation == _PLUS)
        result = a + b;
    else if (operation == _MINUS)
        result = a - b;
    else if (operation == _MULT)
        result = a * b;
    else if (operation == _DIVIDE)
        result = a / b;
    else if (operation == _FACTORIAL)
        result = factorial(a);
    else if (operation == _SQRT)
        result = mysqrt(a);
    else
        std::cout << "incorrect operation" << std::endl;
    return result;
}

// читаем пример от клиента
void read_count(count_request &cr, sockaddr_in from, int &datagram_number) {
    cr.isSlow = true;
    char buffer[BUFFER_SIZE];
    socklen_t from_size = sizeof(from);
    // прочитаем первую чиселку
    bzero(buffer, BUFFER_SIZE);
    if (recvfrom(cr.socket, buffer, sizeof(buffer), 0, (struct sockaddr *) &from, &from_size) < 0) {
        std::cerr << "recvfrom error" << std::endl;
        return;
    }

    std::string first_num(buffer);
    int dg_num = getDatagamNumber(first_num);

    std::cout << "что приняли  : " << dg_num << std::endl;
    std::cout << "что надо было: " << datagram_number << std::endl;
    if (dg_num == datagram_number - 1) {
        // message$number$DUPLICATED
        std::cout << DUPLICATED << std::endl;
        std::string dupl(first_num);
        addDatagramNumber(dupl, dg_num);
        dupl.append(SEPARATOR);
        dupl.append(DUPLICATED);
        if (sendto(cr.socket, dupl.c_str(), BUFFER_SIZE, 0, (sockaddr *) (&from), from_size) < 0) {
            std::cerr << "sendto error" << std::endl;
            return;
        }
    }

    std::cout << "########## datagram_num = " << dg_num << " | message = " << first_num << "\n" << std::endl;
    cr.first_number = std::atof(buffer);
    datagram_number++;

    // прочитаем операцию
    bzero(buffer, BUFFER_SIZE);
    if (recvfrom(cr.socket, buffer, sizeof(buffer), 0, (struct sockaddr *) &from, &from_size) < 0) {
        std::cerr << "recvfrom error" << std::endl;
        return;
    }
    std::string operation = buffer;
    dg_num = getDatagamNumber(operation);
    std::cout << "########## datagram_num = " << dg_num << " | message = " << operation << "\n" << std::endl;
    cr.operation = operation;
    datagram_number++;
    // если операция '+', '-', '*', '/', то надо прочитать вторую чиселку
    // если операция factorial или sqrt, то можно сразу отправлять на вычисление
    if (buffer[0] == '+' || buffer[0] == '-' || buffer[0] == '*' || buffer[0] == '/') {
        // прочитаем вторую чиселку
        bzero(buffer, BUFFER_SIZE);
        if (recvfrom(cr.socket, buffer, sizeof(buffer), 0, (struct sockaddr *) &from, &from_size) < 0) {
            std::cerr << "recvfrom error" << std::endl;
            return;
        }
        std::string sec_num(buffer);
        dg_num = getDatagamNumber(sec_num);
        std::cout << "########## datagram_num = " << dg_num << " | message = " << sec_num << "\n" << std::endl;
        datagram_number++;
        cr.isSlow = false;
        cr.second_number = std::stoi(buffer);
    } else {
        cr.second_number = 0;
    }
}

void processing(count_request &cr, client &cl, sockaddr_in &from) {
    double result = 0;
    std::string request, response;
    socklen_t from_size = sizeof(from);
    if (cr.isSlow) {
        request = cr.operation + "(" + std::to_string(cr.first_number) + ")";
    } else {
        request = std::to_string(cr.first_number) + " " + cr.operation + " " + std::to_string(cr.second_number);
    }
    std::cout << "Client " << cl.ip << ":" << cl.port << " send request: " << request.c_str() << std::endl;
    result = operation_processing(cr.first_number, cr.operation, cr.second_number);
    std::cout << "Server's response: " << result << "\n" << std::endl;
    response = std::to_string(result);
    if (sendto(cr.socket, response.c_str(), BUFFER_SIZE, 0, (sockaddr *) (&from), from_size) < 0) {
        std::cerr << "sendto error" << std::endl;
        return;
    }
}

void server_handler(int server_socket, int datagram_number, std::shared_ptr<ThreadPool> thread_pool) {
    int connection_counter = 0;
    char buffer[BUFFER_SIZE];
    while (true) {
        bool is_mixed = false;
        bool is_duplicated = false;
        count_request cr{};
        struct sockaddr_in from{};
        socklen_t from_size = sizeof(sockaddr_in);
        timeval timeout{TIMEOUT_SEC, TIMEOUT_USEC};

        fd_set server_sockets{};
        FD_ZERO(&server_sockets); // NOLINT
        FD_SET(server_socket, &server_sockets);
        auto &&result = select(server_socket + 1, &server_sockets, nullptr, nullptr, &timeout);

        //cleanup();

        if (result < 0) {
            break;
        }
        if (result == 0 || !FD_ISSET(server_socket, &server_sockets)) {
            continue;
        }

        bzero(buffer, BUFFER_SIZE);

        // что-то приняли и ссмотрим что это
        if (recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr *) &from, &from_size) < 0) {
            std::cerr << "recvfrom error" << std::endl;
            break;
        }

        std::string buf(buffer);
        int dg_num = getDatagamNumber(buf);

        std::cout << "что приняли  : " << dg_num << std::endl;
        std::cout << "что надо было: " << datagram_number << std::endl;

        //  смотрим какая датаграмма пришла - защита от перемешивания датаграмм
        if (dg_num == datagram_number) {
            is_mixed = false;
            is_duplicated = false;
        } else if (dg_num == datagram_number - 1) { // защита на дублирование датаграмм
            // message$number$DUPLICATED
            std::cout << DUPLICATED << std::endl;
            std::string dupl(buffer);
            addDatagramNumber(dupl, dg_num);
            dupl.append(SEPARATOR);
            dupl.append(DUPLICATED);
            if (sendto(server_socket, dupl.c_str(), BUFFER_SIZE, 0, (sockaddr *) (&from), from_size) < 0) {
                std::cerr << "sendto error" << std::endl;
                return;
            }
            is_duplicated = true;
        } else {
            is_mixed = true;
            std::string message(MIXING);
            std::cout << MIXING << std::endl;
            // datagram_number - тот который должен был прийти, dg_num - перемешанный
            addDatagramNumber(message, dg_num);
            addDatagramNumber(message, datagram_number);
            if (sendto(server_socket, message.c_str(), BUFFER_SIZE, 0, (sockaddr *) (&from), from_size) < 0) {
                std::cerr << "sendto error" << std::endl;
                return;
            }
        }

        if (is_mixed) {
            continue;
        }

        if (is_duplicated) {
            continue;
        }

        std::cout << "!########## datagram_num = " << dg_num << " | message = " << buf << "\n" << std::endl;
        datagram_number++;

        client new_client{};
        new_client.ip = inet_ntoa(from.sin_addr);
        new_client.port = ntohs(from.sin_port);
        new_client.from = from;

        if (buf == CHECK_CONNECTION) {
            if (contains(new_client)) {
                std::cout << "We already have this connection" << std::endl;
            } else {
                std::cout << "We have new client" << std::endl;

                new_client.number_connection = connection_counter;

                std::unique_lock<std::mutex> cl_lock(cl_mutex);
                clients.emplace_back(new_client);
                cl_lock.unlock();

                // ответили клиенту, что мы его запомнили
                std::string answer = SUCCESS_CONNECTION;
                if (sendto(server_socket, answer.c_str(), BUFFER_SIZE, 0, (sockaddr *) (&from), from_size) < 0) {
                    std::cerr << "sendto error" << std::endl;
                    return;
                }
                connection_counter++;
                std::cout << "sent response ack to client" << std::endl;
            }
        } else if (buf == _EXIT || buf == "") {
            std::cout << new_client.ip << ":" << new_client.port << " disconnected" << std::endl;
            std::unique_lock<std::mutex> cl_lock(cl_mutex);
            clients.erase(clients.begin() + new_client.number_connection);
            cl_lock.unlock();
        } else if (buf == _TEXT) {
            text(server_socket, new_client, from, datagram_number);
        } else if (buf == _COUNT) {
            std::unique_lock<std::mutex> lock(count_mutex);
            cr.socket = server_socket;
            read_count(cr, from, datagram_number);
            thread_pool->enqueue(processing, cr, new_client, from);
            lock.unlock();
        }
    }
}