#include "Common/Common.hpp"
#include "Network/CameraStream.h"
#include "Network/TCP_Server.h"
#include <IOHandler/ioHandler.h>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <sstream>
#include <chrono>

                                            /* ---------- CONSTRUCTOR / DESTRUCTOR  ----------  */
// Constructor
ioHandler::ioHandler()
{
    this->tcpServer = new Network::TCPServer(Network::IPV::V4, 1234);
    this->cameraServo = new GPIO::ServoInterface(18);

    this->initValidCommands();
    this->initTcpServerCallbacks();

    // // Start TCP Server
    this->startTcpServer();
}

// Destructor
ioHandler::~ioHandler()
{
    for (size_t i = 0; i < this->cameraStreams.size(); i++) {
        delete this->cameraStreams[i];
    }
    delete this->tcpServer;
    delete this->cameraServo;
}
                                            /* ---------- PRIVATE FUNCTIONS ----------  */

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
            std::cout << "Error : Received invalid command." << "\n"; // todo: fix. Do something useful if invalid command was received.
        }

        // TODO: Log the received command:
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
    // TODO: Log all commands registered.
    // TODO: Create command to register user. Only registered users can move the camera, start the UDP server etc. Make different auth levels.


    /* ------ Init cameraPosition ------ */
    // Function call init
    this->functionMap["cam_move"] = [this](const std::vector<std::string>& argv) {
        this->fnMoveCamera(argv);
    };
    // Init cam pos map
    this->cameraPositions["MID"] = cameraPosition::MID;
    this->cameraPositions["MIN"] = cameraPosition::MIN;
    this->cameraPositions["MAX"] = cameraPosition::MAX;

    /* ------ Init UDP Stream ------ */
    this->functionMap["udp_start"] = [this](const std::vector<std::string>& argv) {
        this->fnStartUdpStream(argv); // udp_start <ip> <port>
    };
    /* ------ Stop UDP Stream ------ */
    this->functionMap["udp_stop"] = [this](const std::vector<std::string>& argv) {
        std::cout << "TODO: Implement this" << "\n"; // TODO <=
    };
}


void ioHandler::fnMoveCamera(const std::vector<std::string>& argv) const
{
    /* Usage: cam_move <MIN | MID | MAX > */
    //std::vector<std::string> argv = this->userInput2Vec(v_cmd, " ");

    if (this->cameraPositions.find(argv.at(1)) == this->cameraPositions.end()) {
        std::cout << "Error: " << "Trying to move camera to invalid location: " << argv.at(1) << "\n"; // todo: Log this.
        return;
    }

    switch(this->cameraPositions.at(argv.at(1).c_str())) { // TODO: Log the entries
        case cameraPosition::MID:
            std::cout << "moving camera to MID" << "\n";
            this->cameraServo->moveMid();
            break;
        case cameraPosition::MIN:
            std::cout << "moving camera to MIN" << "\n";
            this->cameraServo->moveMin();
            break;
        case cameraPosition::MAX:
            std::cout << "moving camera to MAX" << "\n";
            this->cameraServo->moveMax();
            break;
        default:
            std::cout << "WARNING: Invalid option..." << "\n";
    }
}


void ioHandler::fnStartUdpStream(const std::vector<std::string>& argv)
{
    /* Usage: udp_start <ip> <port> */
    //this->cameraStreams.push_back(new Network::CameraStream(target_ip, target_port));
    //this->cameraStream = new Network::CameraStream("192.168.99.12", "39009"); // Init with arbitrary address for now. TODO: Start the stream on demand with request from specific IP. TODO: Let port be an int instaed of std::str
    //this->cameraStream->startStream();

    if (argv.size() != 4) {
        std::cout << "Client attempted to run command 'start_udp' with invalid number of arguments: " << argv.size() << "\n"; // todo: save in log
        return;
    }
    std::string target_ip = argv.at(1);
    std::string target_port = argv.at(2);

    // TODO: Associate the server with a userIP/ID. Kill and delete the UDP socket after X minutes/hours etc
    // if there is no activity from the user. Require an interatcion "I am alive" etc every X minutes to keep the server alive.
    // TODO: Sanity check the IP and port provided by user
    std::cout << "Starting UDP Server to target IP " << target_ip << " : " << target_port << "\n";
    this->cameraStreamThreads.push_back(this->getCameraStreamThread(target_ip, target_port));

}

std::thread ioHandler::getCameraStreamThread(const std::string& target_ip, const std::string& target_port) {
    this->cameraStreams.push_back(new Network::CameraStream(target_ip, target_port));
    return std::thread( [this] { this->cameraStreams.back()->startStream(true); } );
}


void ioHandler::startTcpServer()
{
    this->tcpServer->run();
}

int ioHandler::parseTcpMessage(const std::string& msg, const std::string& username)
{
    // input msg: IP:PORT:MSG
    std::cout << "Received message:" << msg << "\n"; // todo: store in log
    // TODO: Require each command to pass a simple authentication step like: send(vcmfd9U@#nvcv2~!fd23g). Expect a returned hash to match from the client.
    // TODO: Check if user is registered. Let only registered users continue

    std::vector<std::string> argv = this->userInput2Vec(msg);
    int ok = false;
    for (auto s = argv.begin(); s != argv.end(); s++) {
        if (algorithm::contains(argv.front(), username)) {
            argv.erase(argv.begin()); // remove header : IP:PORT from msg
            ok = true;
        }
    }
    if (!ok) {
        return -1;
    }

    //debug
    for (auto c = argv.begin(); c != argv.end(); c++) {
        std::cout << "#: " << *c << "\n";
    }

    argv.emplace_back(boost::algorithm::erase_last_copy(argv.back(), "\n"));

    auto check_fn = this->functionMap.find(argv.front());
    if (check_fn != this->functionMap.end()) {
        std::stringstream ss;
        for (auto c = argv.begin(); c != argv.end(); ++c) {
            c != argv.begin() ? ss << " " << *c : ss << *c;
        }
        this->functionMap[argv.front()](argv);
    } else {
        return -1; // Command not found among registered in functionMap (see fn initValidCommands)
    }

    return 0;
}


                                            /* ---------- PUBLIC FUNCTIONS ----------  */

 const std::vector<std::string> ioHandler::userInput2Vec(const std::string& input, const char* separator) const
 {
    std::vector<std::string> argv;
    boost::split(argv, input, algorithm::is_any_of(separator));
    for (auto& i : argv) { // Remove whitespace for each argument
        algorithm::trim(i);
    }

    return argv;
 }
