#include "Common/Common.hpp"
#include "Network/CameraStream.h"
#include "Network/TCP_Server.h"
#include <IOHandler/ioHandler.h>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>

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
        int ret = this->parseTcpMessage(message, client->getUsername());
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
    this->functionMap["cam_move"] = [this](const std::string& v_cmd) {
        // TODO: Log entry etc
        this->fnMoveCamera(v_cmd);
    };

}


void ioHandler::fnMoveCamera(const std::string& v_cmd) const
{
    
    std::vector argv = Common::stringSplit(v_cmd, " ");
    enum position {MID_POS, MIN_POS, MAX_POS};
    static std::map<std::string, position> string_map;
    string_map["mid"] = MID_POS;
    string_map["min"] = MIN_POS;
    string_map["max"] = MAX_POS;

    switch(string_map[argv.at(1).c_str()]) { // TODO: Find a way to make this work.
        case MID_POS:
            std::cout << "moving camera to mid" << "\n";
            break;
        case MIN_POS:
            std::cout << "moving camera to MIN" << "\n";
            break;
        case MAX_POS:
            std::cout << "moving camera to MAX" << "\n";
            break;
        default:
            std::cout << "WARNING: Invalid option..." << "\n";
    }
    this->cameraServo->moveMid();

}

int ioHandler::parseTcpMessage(const std::string& msg, const std::string& username)
{
    std::vector<std::string> res = Common::stringSplit(msg, " ");

    // Validate sender as actual client and pop it from vector
    if (algorithm::contains(res.front(), username)) {
        res.erase(res.begin());
        res.back() = boost::algorithm::erase_tail_copy(res.back(), 1); // Remove trailing new-line char from last element
    } else {
        return -1;
    }

    auto check_fn = this->functionMap.find(res.front());
    if (check_fn != this->functionMap.end()) {
        std::stringstream ss;
        for (auto c = res.begin(); c != res.end(); ++c) {
            c != res.begin() ? ss << " " << *c : ss << *c;
        }
        this->functionMap[res.front()](ss.str());
    } else {
        return -1; // Command not found among registered in functionMap (see fn initValidCommands)
    }

    // std::cout << "Message received : " << msg << "\n"; // TODO: Store in log with timestamp

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