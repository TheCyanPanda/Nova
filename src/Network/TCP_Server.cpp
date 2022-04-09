#include "Common/Common.hpp"
#include <Network/TCP_Server.h>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
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

    void TCPConnection::start(messageHandler&& message_handler, errorHandler&& error_handler)
    {
        this->_messageHandler = std::move(message_handler);
        this->_errorHandler = std::move(error_handler);

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
            // Handle error
            this->_errorHandler();
            return;
        }
        
        // TODO: Log bytesTransferred

        std::stringstream message;
        message << this->_username << ": " << std::istream(&this->_incomingMessages).rdbuf();
        // No need to run .consume on the streambuffer (_incomingMessages) since this is done by .rdbuf()

        // Send message to messageHandler
        this->_messageHandler(message.str());
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
            // Handle error
            this->_errorHandler();
            return;
        }

        // Remove TX message at front of the queue
        this->_outgoingMessages.pop();

        // TODO: Log bytesTransferred

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


                                    // --- Private functions --- //

    const std::string TCPServer::generateUniqueUserId(const uint8_t len)
    {
        std::string user_id;
        do {
            user_id = Common::generateString(len);
        } while (this->hasClient(user_id));

        return user_id;
    }

    void TCPServer::startAccept()
    {
        this->_socket.emplace(this->_ioContext);

        // Accept connection async
        // Dummy socket waits for a connection. When a client connects, move to active connection part.
        // TODO: Refuse connection if number of active connections are at limit
        this->_acceptor.async_accept(*this->_socket, [this](const boost::system::error_code& ec)
        {
            // Create the connection and add to our list of active connections
            auto connection = TCPConnection::create(std::move(*this->_socket));
            this->_activeConnections.insert(connection);

            // Give the connection a uniqe user idx.
            std::string user_id = this->generateUniqueUserId();
            connection->setUserId(user_id);

            // Link unique userId to map
            this->_activeConnectionsMap.insert(std::make_pair(connection->getUserId(), connection));

            if (this->onJoin) {
                this->onJoin(connection);
            }

            // Start up the connection and pass in our message- and error handler
            if (!ec) {
                connection->start(
                    /* Message handler */
                    [this, connection](const std::string& message)
                    {
                        if (this->onClientMessage) {
                            this->onClientMessage(message, connection);
                        }
                    },
                    /* Error handler */
                    [&, weak = std::weak_ptr(connection)]()
                    {
                        // Check connection is still valid and connection object is in our list of active connections
                        if (auto shared = weak.lock(); shared&& this->_activeConnections.erase(shared)) {
                            if (this->onLeave)
                            {
                                this->_activeConnectionsMap.erase(shared->getUserId());
                                this->onLeave(shared);
                            }
                        }
                    }
                    );
            }

            this->startAccept();
        });
    }

                                    // --- Public functions --- //

    int TCPServer::run()
    {
        try
        {
            this->startAccept();
            this->_ioContext.run();
        } catch (std::exception& e) {
            std::cerr << e.what() << "\n";
        }

        return 0;
    }

    // Broadcast message to all connected clients
    void TCPServer::broadcast(const std::string &message)
    {
        for (auto& connection: this->_activeConnections) {
            connection->sendMessage(message);
        }
    }

    int TCPServer::sendToClient(const std::string& client_id, const std::string& message)
    {
        if (this->getActiveClients() == 0 || !(this->hasClient(client_id)))
            return -1;

        this->getClient(client_id)->sendMessage(message);
        return 0;
    }

    bool TCPServer::hasClient(const std::string& clientId)
    {
        if (this->_activeConnectionsMap.count(clientId) > 0)
            return true;

        return false;
    }

    const TCPConnection::pointer TCPServer::getClient(const std::string& clientId)
    {
        if (!(this->hasClient(clientId)))
            return nullptr;

        return this->_activeConnectionsMap[clientId];
    }

}