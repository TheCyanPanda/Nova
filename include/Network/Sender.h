#ifndef SENDER_H
#define SENDER_H

#include <iostream>
#include <chrono>
#include <vector>
#include <Video/CameraDevice.h>

class Sender
{

private:
   CameraDevice* cap;
   cv::Mat out_frame;
   std::vector<int> compression_params;

public:

   // Constructor / Destructor
   Sender();
   ~Sender();

   // Functions
   void sender(const std::string& recv_addr, const std::string& port);

   // Setters

   // Getters
   void foo();

};

#endif
