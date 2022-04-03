#include <Network/Sender.h>
#include <Video/CameraDevice.h>
#include <iostream>
#include <chrono>
#include <map>
#include <thread>

using namespace boost;

// Constructor
Sender::Sender(const std::string& recv_addr, const std::string& port, const int& chunk_sz_)
	: chunk_sz(chunk_sz_)
{
	this->cap = new CameraDevice("/dev/video0");
	this->udp_socket = new UDPSocket(recv_addr, port);
	this->image_buffer = new unsigned char[this->chunk_sz];
}

// Destructor
Sender::~Sender()
{
	delete this->cap;
	delete this->udp_socket;
	delete[] this->image_buffer;
}

// Functions

// TODO: Split this function into separate functions and classes!!
void Sender::sender()
{

	std::vector<unsigned char> buf(this->chunk_sz);
    while (1)
    {

		this->out_frame = this->cap->getFrame();

        if (this->out_frame.empty())
			break; // end of video stream
			// TODO: Do something here. Also create a condition to stop, start and restart the stream over TCP.

        std::vector<uchar> buf;
        cv::imencode(".jpg", this->out_frame, buf, std::vector<int>());

		this->udp_socket->sendBuffer(buf, this->image_buffer, this->chunk_sz);
	
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }

}