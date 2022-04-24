#include <Network/UDP_Server.h>
#include <iostream>

namespace Network
{
    // Constructor
    UDPServer::UDPServer(const std::string& recv_addr_, const std::string& port_)
        : recv_addr(recv_addr_), port(port_)
    {
        this->socket = new asio::ip::udp::socket(this->io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), 0));
        this->resolver = new asio::ip::udp::resolver(this->io_context);
        this->endpoints = this->resolver->resolve(asio::ip::udp::v4(),  this->recv_addr, this->port);

    }


    UDPServer::~UDPServer()
    {
        delete this->socket;
        delete this->resolver;
    }


    void UDPServer::sendBuffer(const std::vector<unsigned char> buf, const size_t buffer_sz, unsigned char* cpy_buffer)
    {
        for (auto it = buf.begin(); it < buf.end(); it += buffer_sz)
        {
            auto end = it + buffer_sz;
            if (end >= buf.end()) {
                end = buf.end();
            }
            if (cpy_buffer != nullptr)
                std::copy(it, end, cpy_buffer);
            this->socket->send_to(asio::buffer(std::string(it, end)),
                (const asio::ip::basic_endpoint<asio::ip::udp> &)*this->endpoints.begin());
        }
    }
}