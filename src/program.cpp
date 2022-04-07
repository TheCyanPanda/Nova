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

    Network::TCPServer Server(Network::IPV::V4, 1234);

    Server.run();

    return 0;
}


