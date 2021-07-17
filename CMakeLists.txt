cmake_minimum_required(VERSION 3.13)
project(tcp_client_server)

find_package(Threads)
find_package(Cellworld)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++20 -pthread")

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
