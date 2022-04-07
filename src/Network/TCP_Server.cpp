#include <Network/TCP_Server.h>
#include <boost/asio/streambuf.hpp>
#include <boost/system/error_code.hpp>
#include <iostream>

namespace Network
{
    /* ------------------------------------ TCP CONNECTION  ------------------------------------*/

    // Constructor
    TCPConnection::TCPConnection(asio::io_context& io_context)
        : _socket(io_context)
    {

    }

    // Private functions
    void TCPConnection::start()
    {
        auto strongThis = shared_from_this();
        asio::async_write(this->_socket, asio::buffer(this->message),
                            [strongThis](const boost::system::error_code& error, size_t bytesTransferred)
                            {
                                if (error){
                                    std::cout << "ERROR: Failed to send message!" << "\n";
                                } else {
                                    std::cout << "Sent" << bytesTransferred << "bytes of data" << "\n";
                                }
                            });


        boost::asio::streambuf buffer;
        this->_socket.async_receive(buffer.prepare(512), [this](const boost::system::error_code& error, size_t bytesTransferred)
        {
            if (error == boost::asio::error::eof) {
                std::cout << "Client disconnected properly!" << "\n";
            } else if (error) {
                std::cout << "Client disconnected in a bad way!" << "\n";
            }
        });
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

    // Private functions
    void TCPServer::startAccept()
    {
        // Create a connection
        auto connection = TCPConnection::create(this->_ioContext);

        this->_activeConnections.push_back(connection); // Add the client to active connections vector

        // Accept connection async
        this->_acceptor.async_accept(connection->getSocket(), [connection, this](const boost::system::error_code& error)
        {
            if (!error){
                connection->start();
            }

            this->startAccept();
        });


    }

}