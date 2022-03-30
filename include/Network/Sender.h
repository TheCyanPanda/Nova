#ifndef SENDER_H
#define SENDER_H

#include <iostream>
#include <Network/InputBuffer.h>

class Sender
{
public:

   // Constructor / Destructor
   Sender();
   ~Sender();

   // Functions
   void sender(const std::string& recv_address);

   // Setters

   // Getters
   void foo();

};

#endif
