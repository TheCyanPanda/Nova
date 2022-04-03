#include <Network/Sender.h>
#include <Video/CameraDevice.h>
#include <iostream>
#include <chrono>
#include <map>
#include <thread>

#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>

using namespace boost;

// Constructor
Sender::Sender()
{
	this->cap = new CameraDevice("/dev/video0");
}

// Destructor
Sender::~Sender()
{

}

// Functions

// TODO: Split this function into separate functions and classes!!
void Sender::sender(const std::string& recv_addr, const std::string& port)
{
    const int chunk_size = 1024;

    asio::io_context io_context;
    asio::ip::udp::socket socket(io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), 0));
    asio::ip::udp::resolver resolver(io_context);
    asio::ip::udp::resolver::results_type endpoints = resolver.resolve(asio::ip::udp::v4(), recv_addr, port);

    uchar buffer[chunk_size];
    while (1)
    {

		this->out_frame = this->cap->getFrame();

        if (this->out_frame.empty())
			break; // end of video stream
			// TODO: Do something here. Also create a condition to stop, start and restart the stream over TCP.

        std::vector<uchar> buf;
        cv::imencode(".jpg", this->out_frame, buf, std::vector<int>());

        for (auto it = buf.begin(); it < buf.end(); it += chunk_size)
        {
            auto end = it + chunk_size;
            if (end >= buf.end())
            {
                end = buf.end();
            }
            std::copy(it, end, buffer);

            socket.send_to(
                boost::asio::buffer(std::string(it, end)),
                (const asio::ip::basic_endpoint<asio::ip::udp> &)*endpoints.begin());
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }

}