cmake_minimum_required(VERSION 3.9)
project(tryudp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -g2 -O0")

add_executable(main main.cpp)
add_executable(server server.cpp constants.h server_functions.h server_commands.h slow_operations.h ThreadPool.h)
add_executable(client client.cpp constants.h client_functions.h)