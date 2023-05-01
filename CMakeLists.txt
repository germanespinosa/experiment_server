cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 20)

project(tcp_client_server
        VERSION 2021.0.1
        DESCRIPTION "Experiment server"
        LANGUAGES CXX)

####
#### DEPENDENCIES
####

if (NOT EXISTS ${CMAKE_BINARY_DIR}/DependenciesConfig.cmake)
    file(DOWNLOAD https://raw.githubusercontent.com/germanespinosa/dependencies/main/DependenciesConfig.cmake ${CMAKE_BINARY_DIR}/DependenciesConfig.cmake)
endif()
set(Dependencies_DIR "${CMAKE_BINARY_DIR}")
find_package (Dependencies REQUIRED)

####
####
####


install_git_dependency(Cellworld
        https://github.com/germanespinosa/cellworld
        PRIVATE
        CMAKE_PROJECT
        IMPORT_TARGETS cellworld)

install_git_dependency(Tcp_messages https://github.com/germanespinosa/tcp-messages
        CMAKE_PROJECT
        IMPORT_TARGETS tcp_messages)

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
        cellworld)

target_link_libraries (tcp_client
        cellworld)
