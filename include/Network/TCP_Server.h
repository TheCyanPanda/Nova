#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <iostream>
#include <optional>
#include <queue>
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/date_time.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

using namespace boost;

class TCPServer
{

private:
    asio::io_service io_service;
    asio::ip::tcp::acceptor* acceptor;
    asio::ip::tcp::socket* socket;

    void accept();


public:
    TCPServer();
    ~TCPServer();

    std::string read();
    void send(const std::string& message);
    int run();
};



#endif /* TCP_SERVER_H */
