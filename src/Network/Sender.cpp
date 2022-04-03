#include <Network/Sender.h>
#include <Video/VideoWindow.h>
#include <Video/CameraDevice.h>
#include <iostream>
#include <chrono>
#include <map>
#include <thread>

// Constructor
Sender::Sender()
{

}

// Destructor
Sender::~Sender()
{

}

// Functions

// TODO: Split this function into separate functions and classes!!
void Sender::sender(const std::string& recv_address_)
{
	try
	{
		asio::io_context ioContext;
		asio::ip::udp::resolver resolver(ioContext);
		asio::ip::udp::socket sender_socket(ioContext);

		sender_socket.open(asio::ip::udp::v4());

		const int recv_port = 39009;
		asio::ip::udp::endpoint recv_endpoint(asio::ip::address::from_string(recv_address_), recv_port);

		// Get video device (webcam)
		//VideoWindow win(0, "cUDP");
		CameraDevice cap("dev/video0");
		VideoWindow cv_window("Test window");
		cv::Mat frame;

		std::vector<int> compression_params;
		compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);

		// this is percent, so 100 == no compression
		compression_params.push_back(50);
		int& compression_rate = compression_params.back();

		std::vector<uint8_t> buffer(20 * MB);
		float fps = 24.f;
		int64_t sent_bytes_per_second = 0;
		//CountdownTimer timer(std::chrono::milliseconds(1000));

		while (1)
		{
			const auto timepoint_before_compression = std::chrono::system_clock::now();
			// Get payload (frame here)
			frame = cap.getFrame();
			if (frame.empty()) {
				std::this_thread::sleep_for(std::chrono::seconds(1));
				continue;
			}

			static int frame_id = 0;
			// Encode frame (cv::imencode)
			cv::imencode(".jpg", frame, buffer, compression_params);
			frame_id++;

			const int16_t parts_num = (buffer.size() + InputBuffer::writable_size() - 1) /
				InputBuffer::writable_size();

			for (int16_t part_id = 0; part_id < parts_num; ++part_id)
			{
				InputBuffer::Header h;
				h.part_id = part_id;
				h.total_parts = parts_num;
				// Size if either MTU or the remainder for the last part
				h.frame_id = frame_id;
				h.part_begin = part_id * InputBuffer::writable_size();
				h.part_size = part_id + 1 == parts_num ? buffer.size() % InputBuffer::writable_size() :
					InputBuffer::writable_size();

				InputBuffer input_buffer;
				input_buffer.set_header(h);
				input_buffer.set_frame_part(
					asio::const_buffer(reinterpret_cast<const char*>(buffer.data()) +
						part_id * InputBuffer::writable_size(), InputBuffer::writable_size()));

				std::error_code err;
				sent_bytes_per_second += sender_socket.send_to(input_buffer.buffer(), recv_endpoint);
				if (err)
				{
					// TODO: Do something
					std::cout << "OOps!" << "\n";
				}
			}

			// Display the frame
			cv::imshow(cv_window.getWindowName(), frame);

			// Get time statistics etc
			
			// Press  ESC on keyboard to  exit
			const char c = static_cast<char>(cv::waitKey(1));
			if (c == 27)
			{
				break;
			}
		}

	}
	catch (const std::exception& e_) {
		std::cout << "Something went wrong!" << "\n";
	}

}