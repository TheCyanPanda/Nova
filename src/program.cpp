#include <Network/Sender.h>
#include <program.hpp>
#include <Network/TCP_Server.h>

int main()
{

#if 0
    // Create UDP Session
    Sender* sender;
    sender = new Sender("192.168.99.12", "39009");

    // UDP stream start
    sender->sender();    // This should be the address of the receiver, TODO: Set via client->TCP

    delete sender;
#endif
    // Test TCP Server
    Network::TCPServer server(Network::IPV::V4, 1234);


    /* Define the TCP server specific callbacks here. TODO: Define these in a separate class */

    // Callback for when a new user connects
    server.onJoin = [](Network::TCPConnection::pointer client)
    {
        std::cout << "User { " << client->getUsername() << " } has joined the server" << "\n";
    };

    // Callback for when a user disconnects
    server.onLeave = [](Network::TCPConnection::pointer client)
    {
        std::cout << "User { " << client->getUsername() << " } has left the server" << "\n";
    };

    // Callback to happen when client sends a message
    server.onClientMessage = [&server](const std::string& message)
    {
        // Parse message

        // Send response to all connected clients
        server.broadcast(message);

    };

    server.run();

    return 0;
}


