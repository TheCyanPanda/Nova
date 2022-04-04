#include <Network/TCP_Server.h>

// Constructor
TCPServer::TCPServer()
{
    this->acceptor = new asio::ip::tcp::acceptor(this->io_service,
        asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 1234));

    this->socket = new asio::ip::tcp::socket(this->io_service);

}

// Destructor
TCPServer::~TCPServer()
{
    delete this->socket;
    delete this->acceptor;
}

// Functions
std::string TCPServer::TCPServer::read()
{
    asio::streambuf buf;
    asio::read_until(*this->socket, buf, "\n");    // Read until "\n" character is seen
    std::string data = asio::buffer_cast<const char*>(buf.data());
    return data;
}

void TCPServer::send(const std::string& message)
{
    const std::string msg = message + "\n";
    asio::write(*this->socket, asio::buffer(message));
}

int TCPServer::run()
{
    this->acceptor->accept(*this->socket);
    std::string message = this->read();
    std::cout << message << "\n";
    this->send("Hello from server!");
    std::cout << "log: Server said hello to client" << "\n";
    return 0;
}