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


struct Location3 : Json_object{
    Json_object_members(
            Add_member(x);
            Add_member(y);
            Add_member(z);
            );
    double x,y,z;
    Location to_location() const{
        return {x,y};
    }
};

struct Rotation3 : Json_object{
    Json_object_members(
            Add_member(roll);
            Add_member(pitch);
            Add_member(yaw);
    );
    double roll,pitch,yaw;
};


struct Agent_state : Json_object {
    Json_object_members(
            Add_member(location);
            Add_member(rotation);
            );
    Location3 location;
    Rotation3 rotation;
};

struct Game_state : Json_object {
    Json_object_members(
            Add_member(episode);
            Add_member(time_stamp);
            Add_member(predator);
            Add_member(prey);
            );
    unsigned int episode;
    double time_stamp;
    Agent_state predator;
    Agent_state prey;
};


struct Game_state_vector : Json_vector<double> {
    Game_state to_game_state() const{
        Game_state gs;
        gs.episode = (unsigned int)(*this)[0];
        gs.time_stamp = (*this)[1];
        gs.prey.location.x = (*this)[2];
        gs.prey.location.y = (*this)[3];
        gs.prey.location.z = (*this)[4];
        gs.prey.rotation.roll = (*this)[5];
        gs.prey.rotation.pitch = (*this)[6];
        gs.prey.rotation.yaw = (*this)[7];
        gs.predator.location.x = (*this)[9];
        gs.predator.location.y = (*this)[10];
        gs.predator.location.z = (*this)[11];
        gs.predator.rotation.roll = (*this)[12];
        gs.predator.rotation.pitch = (*this)[13];
        gs.predator.rotation.yaw = (*this)[14];
        return gs;
    }
} game_state_vector;


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
        server_cmd.content >> game_state_vector;
        client_cmd.command = "update_predator_destination";
        auto game_state = game_state_vector.to_game_state();
        client_cmd.content << game_state.prey.location.to_location();
        std::cout << "New game state: " << game_state << std::endl;
    }

    std::string replyMsg;
    if (Chance::coin_toss(.5)){
        Command set_speed;
        set_speed.command = "update_predator_speed";
        set_speed.content = "3";
        replyMsg << set_speed;
    } else {
        replyMsg << client_cmd;
    }
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
