#ifndef CAMERASTREAM_H
#define CAMERASTREAM_H

#include <iostream>
#include <chrono>
#include <vector>
#include <Video/CameraDevice.h>
#include <Network/UDP_Server.h>

namespace Network {

   class CameraStream
   {

   private:
      CameraDevice* cap;
      cv::Mat out_frame;
      std::vector<int> compression_params;
      std::vector<unsigned char> send_buf;
      const int chunk_sz;
      unsigned char* image_buffer;
      bool started;  // set to false when received stop signal

      Network::UDPServer* udpServer;

   public:
      bool stream_stop;  // True if stream is to be stopped

      // Constructor / Destructor
      CameraStream(const std::string& recv_addr, const std::string& port, const int& chunk_sz_=1024);
      ~CameraStream();

      // Functions
      void startStream(bool start);

      // Setters

      // Getters
      void foo();

   };

}

#endif
