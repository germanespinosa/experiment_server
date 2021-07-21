#include <iostream>
#include <signal.h>
#include <json_cpp.h>
#include <cell_world.h>
#include "../include/tcp_server.h"


using namespace std;
using namespace json_cpp;
using namespace cell_world;

struct Command : Json_object {
    Json_object_members(
            Add_member(command);
            Add_member(content);
            );
    string command;
    string content;
};

struct Game_status : Json_vector<double> {
} game_status;


// declare the server
TcpServer server;

// declare a server observer which will receive incoming messages.
// the server supports multiple observers
server_observer_t observer;

// observer callback. will be called for every new message received by clients
// with the requested IP address
void onIncomingMsg(const Client & client, const char * msg, size_t size) {
    std::string msgStr = msg;
    Command server_cmd;
    Command client_cmd;
    msgStr >> server_cmd;
    if (server_cmd.command == "update_game_state"){
        cout << server_cmd.content << endl;
        server_cmd.content >> game_status;
        client_cmd.command = "update_predator_destination";
        Location preyLocation;
        preyLocation.x = game_status[1];
        preyLocation.y = game_status[2];
        client_cmd.content << preyLocation;
    }
    // print the message
    //game_status.load(msg);
    std::cout << "Observer got client msg: " << msgStr << std::endl;
    // if client sent the string "quit", close server
    // else if client sent "print" print the server clients
    // else just print the client message
//    if (msgStr.find("quit") != std::string::npos) {
//        std::cout << "Closing server..." << std::endl;
//        pipe_ret_t finishRet = server.finish();
//        if (finishRet.success) {
//            std::cout << "Server closed." << std::endl;
//        } else {
//            std::cout << "Failed closing server: " << finishRet.msg << std::endl;
//        }
//    } else if (msgStr.find("print") != std::string::npos){
//        server.printClients();
//    } else {
    std::string replyMsg;
    replyMsg << client_cmd;
    server.sendToAllClients(replyMsg.c_str(), replyMsg.length());
    cout << "sent to game : " << replyMsg << endl;
//    }
}

// observer callback. will be called when client disconnects
void onClientDisconnected(const Client & client) {
    std::cout << "Client: " << client.getIp() << " disconnected: " << client.getInfoMessage() << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        cout << "Wrong parameter." << endl;
        cout << "Usage: ./tcp_server [Port]" << endl;
        exit(1);
    }
    int port = atoi(argv[1]);
    // start server on port 65123
    pipe_ret_t startRet = server.start(port);
    if (startRet.success) {
        std::cout << "Server setup succeeded on port " << port << std::endl;
    } else {
        std::cout << "Server setup failed: " << startRet.msg << std::endl;
        return EXIT_FAILURE;
    }

    // configure and register observer
    observer.incoming_packet_func = onIncomingMsg;
    observer.disconnected_func = onClientDisconnected;
    server.subscribe(observer);

    // receive clients
    while(1) {
        Client client = server.acceptClient(0);
        if (client.isConnected()) {
            std::cout << "Got client with IP: " << client.getIp() << std::endl;
            server.printClients();
        } else {
            std::cout << "Accepting client failed: " << client.getInfoMessage() << std::endl;
        }
        sleep(1);
    }

    return 0;
}
