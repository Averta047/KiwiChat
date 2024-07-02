#include <iostream>
#include <string>
#include <enet/enet.h>

int main(int argc, char** argv) {
    if (enet_initialize() != 0) {
        std::cerr << "An error occurred while initializing ENet.\n";
        return EXIT_FAILURE;
    }

    ENetHost* client;
    client = enet_host_create(NULL, 1, 2, 0, 0);
    if (!client) {
        std::cerr << "An error occurred while trying to create an ENet client host.\n";
        return EXIT_FAILURE;
    }

    ENetAddress address;
    ENetPeer* peer;
    enet_address_set_host(&address, "localhost");
    address.port = 1234;

    peer = enet_host_connect(client, &address, 2, 0);
    if (!peer) {
        std::cerr << "No available peers for initiating an ENet connection.\n";
        return EXIT_FAILURE;
    }

    ENetEvent event;
    if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
        std::cout << "Connection to localhost:1234 succeeded.\n";
    }
    else {
        enet_peer_reset(peer);
        std::cerr << "Connection to localhost:1234 failed.\n";
        return EXIT_FAILURE;
    }

    std::string username;
    std::cout << "username: ";
    std::getline(std::cin, username);
    std::cout << "\n";
    ENetPacket* packet = enet_packet_create(username.c_str(),
        username.size() + 1,
        ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
    enet_host_flush(client);

    std::string message;
    while (true) {
        std::getline(std::cin, message);
        if (message == "disconnect") {
            enet_peer_disconnect(peer, 0);
            while (enet_host_service(client, &event, 3000) > 0) {
                switch (event.type) {
                case ENET_EVENT_TYPE_RECEIVE:
                    enet_packet_destroy(event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    std::cout << "Disconnection succeeded.\n";
                    break;
                }
            }
            break;
        }
        else {
            ENetPacket* packet = enet_packet_create(message.c_str(),
                message.size() + 1,
                ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(peer, 0, packet);
            enet_host_flush(client);
        }
    }

    enet_host_destroy(client);
    enet_deinitialize();

    return EXIT_SUCCESS;
}
