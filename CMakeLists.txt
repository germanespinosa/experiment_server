cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 20)

project(tcp_client_server
        VERSION 2021.0.1
        DESCRIPTION "Experiment server"
        LANGUAGES CXX)

find_package(Cellworld REQUIRED)
find_package(Threads)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread")

add_executable(tcp_server
        tool/server_example.cpp
        src/tcp_client.cpp
        src/tcp_server.cpp
        src/client.cpp)

add_executable(tcp_client
        tool/client_example.cpp
        src/tcp_client.cpp
        src/tcp_server.cpp
        src/client.cpp)

target_link_libraries (tcp_server
        ${CMAKE_THREAD_LIBS_INIT}
        ${CELLWORLD_LIBRARIES})

target_link_libraries (tcp_client
        ${CMAKE_THREAD_LIBS_INIT}
        ${CELLWORLD_LIBRARIES})
