#include "opencv2/core.hpp"
#include <Network/Sender.h>
#include <program.hpp>
#include <Network/TCP_Server.h>
#include <boost/algorithm/string.hpp>
#include <GPIO/gpio.h>

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
#if 0
    // Test TCP Server
    Network::TCPServer server(Network::IPV::V4, 1234);


    /* Define the TCP server specific callbacks here. TODO: Define these in a separate class */

    // Callback for when a new user connects
    server.onJoin = [&](Network::TCPConnection::pointer client)
    {
        std::cout << "User { " << client->getUsername() << " } has joined the server" << "\n";
    };

    // Callback for when a user disconnects
    server.onLeave = [](Network::TCPConnection::pointer client)
    {
        std::cout << "User { " << client->getUsername() << " } has left the server" << "\n";
    };

    // Callback to happen when client sends a message
    server.onClientMessage = [&server](const std::string& message, Network::TCPConnection::pointer client)
    {
        // Parse message

        // Send response to all connected clients
        std::stringstream ss;
        ss << client->getUserId() << " : " << client->getUsername() << " Sent message: " << message;
        server.broadcast(ss.str());

        // Send message back to client
        client->sendMessage("You sent me a message\n");
        if (algorithm::contains(ss.str(), "\\q")){
            std::cout << "hey !" << "\n";
            client->disconnect();
        }
    };

    server.run();
#endif

    // Servo test
    IO::testServo tServo(16);
    //std::cout << "ret: " << ret << "\n";

    return 0;
}


