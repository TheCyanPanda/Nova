#include <IOHandler/ioHandler.h>

int main()
{
    std::cout << "0" << "\n";
    ioHandler* h_io;
    h_io = new ioHandler();

    delete h_io;


    return 0;
}


