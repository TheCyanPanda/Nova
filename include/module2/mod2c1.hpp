#ifndef INCLUDE_MOD2C1_HPP
#define INCLUDE_MOD2C1_HPP

#include <iostream>

#define FRAME_HEIGHT 720
#define FRAME_WIDTH 1280
#define FRAME_INTERVAL (1000/30)
#define PACK_SIZE 4096 //udp pack size; note that OSX limits < 8100 bytes
#define ENCODE_QUALITY 80

class mod2c1
{
public:

   void foo();

};

#endif
