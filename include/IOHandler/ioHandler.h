#ifndef IO_HANDLER_H
#define IO_HANDLER_H

#include <Network/TCP_Server.h>
#include <Network/CameraStream.h>
#include <GPIO/gpio.h>


class ioHandler
{
private:
    enum class Commands {CAMERA_MOVE};
    enum class CameraCommands {LEFT, MID, RIGHT};

    Network::CameraStream* cameraStream;
    Network::TCPServer* tcpServer;
    GPIO::ServoInterface* cameraServo;

    const std::string commandDelimiter = "::";
    std::map<Commands, std::string> validCommands;
    std::map<std::string, CameraCommands> camServoCommands;

    void initTcpServerCallbacks();
    void initValidCommands();
    int parseTcpMessage(const std::string& msg);
    void startTcpServer();
    void startUdpServer();

public:
    ioHandler();
    ~ioHandler();
};

#endif /* IO_HANDLER_H */
