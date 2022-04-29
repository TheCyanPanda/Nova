#ifndef IO_HANDLER_H
#define IO_HANDLER_H

#include <Network/TCP_Server.h>
#include <Network/CameraStream.h>
#include <GPIO/gpio.h>


class ioHandler
{
private:
    // Variables
    enum class cameraPosition {MID, MIN, MAX};
    std::vector<Network::CameraStream*> cameraStreams;
    Network::TCPServer* tcpServer;
    GPIO::ServoInterface* cameraServo;

    std::map<std::string, cameraPosition> cameraPositions;
    std::map<std::string, std::function<void(const std::string&)>> functionMap;

        // Init functions
    void initTcpServerCallbacks();
    void initValidCommands();

    // Callable over TCP
    void fnMoveCamera(const std::string& v_cmd) const;
    void fnStartUdpStream(const std::string v_cmd);

    // Functions
    int parseTcpMessage(const std::string& msg, const std::string& username);
    void startTcpServer();

public:
    ioHandler();
    ~ioHandler();

    const std::vector<std::string> userInput2Vec(const std::string& input, const char* separator=" ") const;
};

#endif /* IO_HANDLER_H */
