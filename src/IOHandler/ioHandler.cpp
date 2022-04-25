#include "Common/Common.hpp"
#include "Network/CameraStream.h"
#include "Network/TCP_Server.h"
#include <IOHandler/ioHandler.h>
#include <boost/algorithm/string.hpp>

// Constructor
ioHandler::ioHandler()
{
    this->cameraStream = new Network::CameraStream("192.168.99.12", "39009"); // Init with arbitrary address for now. TODO: Start the stream on demand with request from specific IP. TODO: Let port be an int instaed of std::str
    this->tcpServer = new Network::TCPServer(Network::IPV::V4, 1234);
    this->cameraServo = new GPIO::ServoInterface(18);

    this->initValidCommands();
    this->initTcpServerCallbacks();

    // UDP stream start
    // this->startUdpServer();

    // // Test Servo
    this->cameraServo->moveMid();

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
    this->tcpServer->onJoin = [](Network::TCPConnection::pointer client)
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
        int ret = this->parseTcpMessage(message);
        if (ret) {
            std::cout << "Error..." << "\n"; // todo: fix.
        }

        // Send response to all connected clients
        std::stringstream ss;
        ss << client->getUserId() << " : " << client->getUsername() << " Sent message: " << message;
        //this->tcpServer->broadcast(ss.str()); // todo: don't do this!

        // Send message back to client
        client->sendMessage("You sent me a message\n");
        // if (algorithm::contains(ss.str(), "\\q")){
        //     std::cout << "hey !" << "\n";
        //     client->disconnect();
        // }
    };

}

void ioHandler::initValidCommands()
{
    // cam_set: <mid/left/right>
    this->validCommands.insert(std::make_pair(Commands::CAMERA_MOVE, "cam_move"));
    this->camServoCommands.insert(std::make_pair("left", CameraCommands::LEFT));
    this->camServoCommands.insert(std::make_pair("mid", CameraCommands::MID));
    this->camServoCommands.insert(std::make_pair("right", CameraCommands::RIGHT));


}

int ioHandler::parseTcpMessage(const std::string& msg)
{
    std::vector<std::string> res = Common::stringSplit(msg, ": ");

    for (auto i : res) {
        if (algorithm::contains(i, this->validCommands[Commands::CAMERA_MOVE]))
        {
            std::cout << i << "\n";
            size_t sub_c_idx = i.find(this->commandDelimiter);
            if (sub_c_idx != std::string::npos) { // TODO: Make this work
                std::string command =  i.substr(sub_c_idx + this->commandDelimiter.length(), i.length() - sub_c_idx);
                CameraCommands token = this->camServoCommands[command];
                switch (token)
                {
                    case CameraCommands::LEFT:
                        std::cout << "vEnster!" << "\n";
                        break;
                    case CameraCommands::MID:
                        std::cout << "mitten!" << "\n";
                        break;
                    case CameraCommands::RIGHT:
                        std::cout << "hoeger" << "\n";
                        break;
                    default:
                        std::cout << "Invalid command" << "\n";
                }
            }
        }
    }

    std::cout << "Message received : " << msg << "\n";

    return 0;
}


void ioHandler::startTcpServer()
{
    this->tcpServer->run();
}

void ioHandler::startUdpServer()
{
    this->cameraStream->startStream();
}