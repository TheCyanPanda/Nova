#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <string>
#include <vector>
#include <optional>
#include <unordered_set>
#include <queue>
#include <limits>
#include <functional>


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
        std::string _username;

        explicit TCPConnection(tcp::socket&& socket);

        // Queue written to when we write a message
        std::queue<std::string> _outgoingMessages; // queue - TODO: Rename this to e.g. TXQueue, TXBuffer etc;.
        // Streambuffer for incoming messages: _streamBuf
        asio::streambuf _incomingMessages {63553}; // TODO: Initialize this with given size in constructor with std::numeric_limits<uint16_t>::max() : TODO: Rename this to something more appropriate like e.g. rxBuffer etc

        void asyncRead();
        void onRead(system::error_code ec, size_t bytesTransferred);
        void asyncWrite();
        void onWrite(system::error_code ec, size_t bytesTransferred);


    public:
        using pointer = std::shared_ptr<TCPConnection>;
        static pointer create(tcp::socket&& socket) { return pointer(new TCPConnection(std::move(socket))); }
        void start();
        void sendMessage(const std::string& message); // Post
        tcp::socket& getSocket() { return this->_socket; };

        inline const std::string& getUsername() const {return this->_username;}

    };

    class TCPServer
    {
        using onJoinHandler = std::function<void(TCPConnection::pointer)>; // Trigger handler when someone connects
        using onLeaveHandler = std::function<void(TCPConnection::pointer)>; // Trigger when user disconnects
        using OnClientMessageHandler = std::function<void(std::string)>; // Trigger when a client sends message

    private:
        IPV _ipVersion;
        int _port;
        asio::io_context _ioContext;
        tcp::acceptor _acceptor;
        std::optional<tcp::socket> _socket; // c++17
        std::unordered_set<TCPConnection::pointer> _activeConnections {};

        void startAccept();

    public:
        onJoinHandler onJoin;
        onLeaveHandler onLeave;
        OnClientMessageHandler onClientMessage;

        TCPServer(IPV ipv, int port);
        ~TCPServer();

        int run();
        void broadcast(const std::string& message); // Send message to ALL connected clients

    };


}



#endif /* TCP_SERVER_H */
