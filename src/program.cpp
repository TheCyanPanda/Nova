#include <Network/Sender.h>
#include <program.hpp>

#include <opencv4/opencv2/highgui.hpp>
#include <boost/asio.hpp>

int main()
{

    // Create UDP Session
    Sender* sender;
    sender = new Sender();

    // UDP stream start
    sender->sender("127.0.0.1", "39009");    // This should be the address of the receiver, TODO: Set via client->TCP



    delete sender;
    return 0;
}
