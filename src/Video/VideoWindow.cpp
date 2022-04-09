#include <Video/VideoWindow.h>

VideoWindow::VideoWindow(const std::string& window_name)
   : _window_name(window_name)
   {
      if (window_name.empty())
      {
         throw std::runtime_error("ERROR: VideoWindow must have a name");
      }

      cv::namedWindow(this->_window_name.c_str(), cv::WINDOW_AUTOSIZE);

   }


VideoWindow::~VideoWindow()
{
   try
   {
      cv::destroyWindow(this->_window_name);
   }

   catch (const std::exception& e_)
   {

   std::cerr << "Exception while destroying window " << this->_window_name << ". ERROR: " << e_.what() << "\n";
   }
}
