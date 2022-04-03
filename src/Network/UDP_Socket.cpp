#include <Network/UDP_Socket.h>
#include <iostream>

// Constructor
UDPSocket::UDPSocket(const std::string& recv_addr_, const std::string& port_)
    : recv_addr(recv_addr_), port(port_)
{
    this->socket = new asio::ip::udp::socket(this->io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), 0));
    this->resolver = new asio::ip::udp::resolver(this->io_context);
    this->endpoints = this->resolver->resolve(asio::ip::udp::v4(),  this->recv_addr, this->port);

}


UDPSocket::~UDPSocket()
{
    delete this->socket;
    delete this->resolver;
}


void UDPSocket::sendBuffer(const std::vector<unsigned char> buf, unsigned char* buffer, const size_t buffer_sz)
{
    for (auto it = buf.begin(); it < buf.end(); it += buffer_sz)
    {
        auto end = it + buffer_sz;
        if (end >= buf.end()) {
            end = buf.end();
        }
        std::copy(it, end, buffer);
        this->socket->send_to(asio::buffer(std::string(it, end)),
        (const asio::ip::basic_endpoint<asio::ip::udp> &)*this->endpoints.begin());
    }
}