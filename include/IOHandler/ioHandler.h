#ifndef IO_HANDLER_H
#define IO_HANDLER_H

#include <Network/TCP_Server.h>
#include <Network/CameraStream.h>
#include <GPIO/gpio.h>


class ioHandler
{
private:
    Network::CameraStream* cameraStream;
    Network::TCPServer* tcpServer;
    GPIO::ServoInterface* cameraServo;

    void initTcpServerCallbacks();
    void onTCPClientMessageCallback(const std::string& message, Network::TCPConnection::pointer client);
    void parseTcpMessage();
    void startTcpServer();
    void startUdpServer(); // todo; implement this

public:
    ioHandler();
    ~ioHandler();
};

#endif /* IO_HANDLER_H */
