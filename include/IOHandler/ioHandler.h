#ifndef IO_HANDLER_H
#define IO_HANDLER_H

#include <Network/TCP_Server.h>
#include <Network/CameraStream.h>
#include <GPIO/gpio.h>
#include <thread>


class ioHandler
{
private:
    // Variables
    enum class cameraPosition {MID, MIN, MAX};
    std::vector<Network::CameraStream*> cameraStreams;
    std::vector<std::thread> cameraStreamThreads;   // todo: make this into a map or something so that we can keep track of the streams and terminate
    Network::TCPServer* tcpServer;
    GPIO::ServoInterface* cameraServo;

    std::map<std::string, cameraPosition> cameraPositions;
    std::map<std::string, std::function<void(const std::vector<std::string>&)>> functionMap;

    // Init functions
    void initTcpServerCallbacks();
    void initValidCommands();

    // Callable over TCP
    void fnMoveCamera(const std::vector<std::string>& argv) const;
    void fnStartUdpStream(const std::vector<std::string>& argv);

    // Functions
    int parseTcpMessage(const std::string& msg, const std::string& username);
    void startTcpServer();

    std::thread getCameraStreamThread(const std::string& target_ip, const std::string& target_port);

public:
    ioHandler();
    ~ioHandler();

    const std::vector<std::string> userInput2Vec(const std::string& input, const char* separator=" ") const;
};

#endif /* IO_HANDLER_H */
