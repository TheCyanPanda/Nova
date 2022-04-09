#include <Network/TCP_Server.h>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/system/error_code.hpp>
#include <iostream>

namespace Network
{
    /* ------------------------------------ TCP CONNECTION  ------------------------------------*/

    // Constructor
    TCPConnection::TCPConnection(tcp::socket&& socket)
        : _socket(std::move(socket))
    {
        system::error_code error_code;
        std::stringstream name;
        name << this->_socket.remote_endpoint();  // tee remote_endpoint into name
        this->_username = name.str();
    }


                                // --- Private functions --- //

    void TCPConnection::start( /* callbacks as args */)
    {
        this->asyncRead();
    }

    // Waits for a new message from client
    void TCPConnection::asyncRead()
    {
        // Wait for a thread (ioContext)
        asio::async_read_until(this->_socket, this->_incomingMessages, "\n", [self = shared_from_this()] (system::error_code ec, size_t bytesTransferred) 
        {
            self->onRead(ec, bytesTransferred);
        });
    }

    // Executed when message in queue
    void TCPConnection::onRead(system::error_code ec, size_t bytesTransferred)
    {
        if (ec)
        {
            this->_socket.close(ec); // TODO: check (if (ec)) if there was an error upon closing socket
            // TODO: Add an error handler
            return;
        }

        // TODO: Pop this message out to whetever process is needed. For now, just print out.
        std::stringstream message;
        message << this->_username << ": " << std::istream(&this->_incomingMessages).rdbuf();
        // No need to run .consume on the streambuffer (_incomingMessages) since this is done by .rdbuf()

        std::cout << message.str();
        // TODO: Add message handler

        // Repeat this process to handle the next RX message in the buffer:
        this->asyncRead();
    }

    void TCPConnection::asyncWrite()
    {
        // Wait for a thread (ioContext)
        asio::async_write(this->_socket, asio::buffer(this->_outgoingMessages.front()) , [self = shared_from_this()] (system::error_code ec, size_t bytesTransferred)
        {
            self->onWrite(ec, bytesTransferred);
        });
    }

    void TCPConnection::onWrite(system::error_code ec, size_t bytesTransferred)
    {
        if (ec)
        {
            this->_socket.close(ec); // TODO: check (if (ec)) if there was an error upon closing socket
            // TODO: Add an error handler
            return;
        }

        // Remove TX message at front of the queue
        this->_outgoingMessages.pop();
        // Repeat this process if there are more messages in the queue:
        if (!this->_outgoingMessages.empty()) {
            this->asyncWrite();
        }

    }

                                // --- Public functions --- //
    // Post
    void TCPConnection::sendMessage(const std::string &message)
    {
        bool queue_idle = this->_outgoingMessages.empty();
        this->_outgoingMessages.push(message);

        if (queue_idle) {
            this->asyncWrite();
        }
    }

    /* -------------------------------------- TCP SERVER  --------------------------------------*/

    // Constructor
    TCPServer::TCPServer(IPV ipv, int port)
        : _ipVersion(ipv), _port(port), _acceptor(_ioContext, tcp::endpoint(ipv == IPV::V4 ? tcp::v4() : tcp::v6(), port))

    {

    }


    // Destructor
    TCPServer::~TCPServer()
    {

    }


    // Public functions
    int TCPServer::run()
    {
        try
        {
            this->startAccept();
            this->_ioContext.run(); // blocking method
        } catch (std::exception& e) {
            std::cerr << e.what() << "\n";
        }

        return 0;
    }

                                    // --- Private functions --- //

    void TCPServer::startAccept()
    {
        this->_socket.emplace(this->_ioContext);

        // Accept connection async
        // Dummy socket waits for a connection. When a client connects, move to active connection part.
        this->_acceptor.async_accept(*this->_socket, [this](const boost::system::error_code& ec)
        {
            // Create the connection and add to our list of active connections
            auto connection = TCPConnection::create(std::move(*this->_socket));
            this->_activeConnections.insert(connection);

            if (this->onJoin) {
                this->onJoin(connection);
            }

            if (!ec) {
                connection->start();
            }

            this->startAccept();
        });
    }

                                    // --- Public functions --- //

    void TCPServer::broadcast(const std::string &message)
    {
        for (auto& connection: this->_activeConnections) {
            connection->sendMessage(message);
        }
    }

}