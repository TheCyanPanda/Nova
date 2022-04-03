#ifndef SENDER_H
#define SENDER_H

#include <iostream>
#include <chrono>
#include <vector>
#include <Video/CameraDevice.h>
#include <Network/UDP_Socket.h>

class Sender
{

private:
   CameraDevice* cap;
   cv::Mat out_frame;
   std::vector<int> compression_params;
   std::vector<unsigned char> send_buf;
   const int chunk_sz;
   unsigned char* image_buffer;

   UDPSocket* udp_socket;

public:

   // Constructor / Destructor
   Sender(const std::string& recv_addr, const std::string& port, const int& chunk_sz_=1024);
   ~Sender();

   // Functions
   void sender();

   // Setters

   // Getters
   void foo();

};

#endif
