#include <iostream>
#include <enet/enet.h>

int main(int argc, char** argv) {
    if (enet_initialize() != 0) {
        std::cerr << "An error occurred while initializing ENet.\n";
        return EXIT_FAILURE;
    }

    ENetAddress address;
    ENetHost* server;
    address.host = ENET_HOST_ANY;
    address.port = 1234;

    server = enet_host_create(&address, 32, 2, 0, 0);
    if (!server) {
        std::cerr << "An error occurred while trying to create an ENet server host.\n";
        return EXIT_FAILURE;
    }

    std::cout << "Server started on port 1234.\n";

    while (true) {
        ENetEvent event;
        while (enet_host_service(server, &event, 10) > 0) {
            switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                std::cout << "A new client connected from "
                    << event.peer->address.host << ":"
                    << event.peer->address.port << ".\n";
                event.peer->data = (void*)"Client information";
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                std::cout << "Packet received from "
                    << (char*)event.peer->data
                    << ": " << event.packet->data << "\n";
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << (char*)event.peer->data << " disconnected.\n";
                event.peer->data = NULL;
                break;
            default:
                break;
            }
        }

        // Check for disconnected clients manually
        for (size_t i = 0; i < server->peerCount; ++i) {
            ENetPeer* peer = &server->peers[i];
            if (peer->state == ENET_PEER_STATE_DISCONNECTED) {
                if (peer->data != NULL) {
                    std::cout << (char*)peer->data << " disconnected unexpectedly.\n";
                    peer->data = NULL;
                }
            }
        }
    }

    enet_host_destroy(server);
    enet_deinitialize();

    return EXIT_SUCCESS;
}
