#include "Network/CameraStream.h"
#include "Network/TCP_Server.h"
#include <IOHandler/ioHandler.h>
#include <boost/algorithm/string.hpp>

// Constructor
ioHandler::ioHandler()
{
    std::cout << "1" << "\n"; // debug
    this->cameraStream = new Network::CameraStream("192.168.99.12", "39009"); // Init with arbitrary address for now. TODO: Start the stream on demand with request from specific IP. TODO: Let port be an int instaed of std::str
    this->tcpServer = new Network::TCPServer(Network::IPV::V4, 1234);
    this->cameraServo = new GPIO::ServoInterface(18);
    std::cout << "2" << "\n"; // debug

    this->initTcpServerCallbacks();

    // UDP stream start
    // this->startUdpServer();

    // // Test Servo
    this->cameraServo->moveMid();
    std::cout << "2" << "\n"; // debug

    // // Start TCP Server
    this->startTcpServer();
}

// Destructor
ioHandler::~ioHandler()
{
    delete this->cameraStream;
    delete this->tcpServer;
    delete this->cameraServo;
}

void ioHandler::initTcpServerCallbacks()
{
    // Callback for when a new user connects
    this->tcpServer->onJoin = [&](Network::TCPConnection::pointer client)
    {
        std::cout << "User { " << client->getUsername() << " } has joined the server" << "\n";
    };

    // Callback for when a user disconnects
    this->tcpServer->onLeave = [](Network::TCPConnection::pointer client)
    {
        std::cout << "User { " << client->getUsername() << " } has left the server" << "\n";
    };

    // Callback to happen when client sends a message
    this->tcpServer->onClientMessage = [this](const std::string& message, Network::TCPConnection::pointer client)
    {
    // Parse message

        // Send response to all connected clients
        std::stringstream ss;
        ss << client->getUserId() << " : " << client->getUsername() << " Sent message: " << message;
        this->tcpServer->broadcast(ss.str()); // todo: don't do this!

        // Send message back to client
        client->sendMessage("You sent me a message\n");
        if (algorithm::contains(ss.str(), "\\q")){
            std::cout << "hey !" << "\n";
            client->disconnect();
        }
    };
}

void ioHandler::onTCPClientMessageCallback(const std::string& message, Network::TCPConnection::pointer client)
{
    // Parse message

    // Send response to all connected clients
    std::stringstream ss;
    ss << client->getUserId() << " : " << client->getUsername() << " Sent message: " << message;
    this->tcpServer->broadcast(ss.str()); // todo: don't do this!

    // Send message back to client
    client->sendMessage("You sent me a message\n");
    if (algorithm::contains(ss.str(), "\\q")){
        std::cout << "hey !" << "\n";
        client->disconnect();
    }
}

void ioHandler::startTcpServer()
{
    this->tcpServer->run();
}

void ioHandler::startUdpServer()
{
    this->cameraStream->startStream();
}