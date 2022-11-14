#include "Network/TCP_Server.h"
#include <Network/CameraStream.h>
#include <Video/CameraDevice.h>
#include <iostream>
#include <chrono>
#include <map>
//#include <thread>
#include <boost/thread.hpp>

using namespace boost;

namespace Network
{

	// Constructor
	CameraStream::CameraStream(const std::string& recv_addr, const std::string& port, const int& chunk_sz_)
		: chunk_sz(chunk_sz_)
	{
		this->stream_stop = false;
		this->cap = new CameraDevice("/dev/video0");
		this->udpServer = new Network::UDPServer(recv_addr, port);
		this->image_buffer = new unsigned char[this->chunk_sz];
	}

	// Destructor
	CameraStream::~CameraStream()
	{
		delete this->cap;
		delete this->udpServer;
		delete[] this->image_buffer;
		this->image_buffer = 0;
	}

	// Functions

	// TODO: Split this function into separate functions and classes!!
	void CameraStream::startStream(bool start)
	{
		std::cout << "Starting stream..." << "\n";
		// Create a buffer to hold the encoded image fragments
		std::vector<unsigned char> buf(this->chunk_sz);

		// Main loop
		while (1)
		{
			if (this->stream_stop) {
				break;
			}
			this->out_frame = this->cap->getFrame();

			if (this->out_frame.empty())
				break; // end of video stream
				// TODO: Do something here. Also create a condition to stop, start and restart the stream over TCP.

			std::vector<uchar> buf;
			cv::imencode(".jpg", this->out_frame, buf, std::vector<int>());

			this->udpServer->sendBuffer(buf, this->chunk_sz);

			std::this_thread::sleep_for(std::chrono::milliseconds(30));
		}

	}

}