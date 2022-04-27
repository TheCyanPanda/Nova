#ifndef IO_HANDLER_H
#define IO_HANDLER_H

#include <Network/TCP_Server.h>
#include <Network/CameraStream.h>
#include <GPIO/gpio.h>


class ioHandler
{
private:
    enum class Commands {CAMERA_MOVE};

    Network::CameraStream* cameraStream;
    Network::TCPServer* tcpServer;
    GPIO::ServoInterface* cameraServo;

    std::map<std::string, std::function<void(const std::string&)>> functionMap;
    void fnMoveCamera(const std::string& v_cmd) const;

    void initTcpServerCallbacks();
    void initValidCommands();
    int parseTcpMessage(const std::string& msg, const std::string& username);
    void startTcpServer();
    void startUdpServer();

public:
    ioHandler();
    ~ioHandler();
};

#endif /* IO_HANDLER_H */
