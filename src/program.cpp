#include <socket/PracticalSocket.hpp> // Socket
#include <socket/mod1c2.hpp>
#include <module2/mod2c1.hpp> // Server config
#include <module2/mod2c2.hpp>
#include <program.hpp>

#include <opencv4/opencv2/highgui.hpp>

int main()
{

   cv::Mat image;
   UDPSocket sock(3333);

   mod1c2 m1c2;
   mod2c1 m2c1;
   mod2c2 m2c2;

   m1c2.foo();
   m2c1.foo();
   m2c2.foo();

   program::foo();

   return 0;
}
