#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

#include <exception>
#include <string>
#include <iostream>

#include "opencv2/opencv.hpp"

class VideoWindow
{
private:
    std::string _window_name;

protected:

public:
    // Constructor / Destructor
    VideoWindow(const std::string& window_name);
    ~VideoWindow();

    // Functions
    std::string getWindowName() const { return this->_window_name; }
    
};

#endif