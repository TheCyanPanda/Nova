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
    server.onJoin = [](Network::TCPConnection::pointer server)
    {
        std::cout << "User { " << server->getUsername() << " } has joined the server" << "\n";
    };
    server.onLeave = [](Network::TCPConnection::pointer server)
    {

    };

    server.onClientMessage = [](const std::string& message)
    {

    };

    server.run();

    return 0;
}


