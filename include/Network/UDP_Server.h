#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>

using namespace boost;

namespace Network
{
    class UDPServer
    {

    private:
        asio::io_context io_context;
        asio::ip::udp::socket* socket;
        asio::ip::udp::resolver* resolver;
        asio::ip::udp::resolver::results_type endpoints;

        std::string recv_addr;
        std::string port;

    protected:

    public:
        UDPServer(const std::string& recv_addr_, const std::string& port_);
        ~UDPServer();

        void sendBuffer(const std::vector<unsigned char> buf, const size_t buffer_sz, unsigned char* cpy_buffer=nullptr);

    };
}

#endif /* UDP_SERVER_H */
