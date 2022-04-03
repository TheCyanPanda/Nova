#ifndef UDP_SOCKET_H
#define UDP_SOCKET_H

#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>

using namespace boost;

class UDPSocket
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
    UDPSocket(const std::string& recv_addr_, const std::string& port_);
    ~UDPSocket();

    void sendBuffer(const std::vector<unsigned char> buf, const size_t buffer_sz, unsigned char* cpy_buffer=nullptr);

};

   
   

#endif /* UDP_SOCKET_H */
