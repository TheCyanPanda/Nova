#include <Network/Sender.h>
#include <program.hpp>

int main()
{

    // Create UDP Session
    Sender* sender;
    sender = new Sender("192.168.99.12", "39009");

    // UDP stream start
    sender->sender();    // This should be the address of the receiver, TODO: Set via client->TCP

    delete sender;
    return 0;
}
