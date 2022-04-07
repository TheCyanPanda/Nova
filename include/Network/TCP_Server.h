#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <string>
#include <vector>


using namespace boost;

namespace Network
{
    using asio::ip::tcp;

    enum class IPV
    {
        V4,
        V6
    };


    class TCPConnection
        : public std::enable_shared_from_this<TCPConnection>
    {
    private:
        tcp::socket _socket;
        std::string message {"Hello from server!"};
        explicit TCPConnection(asio::io_context& io_context);

    public:
        using pointer = std::shared_ptr<TCPConnection>;
        static pointer create(asio::io_context& io_context) {
            return pointer(new TCPConnection(io_context));
        }
        void start();
        tcp::socket& getSocket() { return this->_socket; };

    };

    class TCPServer
    {
    private:
        IPV _ipVersion;
        int _port;
        asio::io_context _ioContext;
        tcp::acceptor _acceptor;
        std::vector<TCPConnection::pointer> _activeConnections {};

        void startAccept();

    public:
        TCPServer(IPV ipv, int port);
        ~TCPServer();

        int run();
        void writeToConnection(int connection_index, std::string message);
        void registerListenCallback(int connection_index);

    };


}



#endif /* TCP_SERVER_H */
