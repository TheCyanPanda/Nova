#ifndef CAMERA_DEVICE_H
#define CAMERA_DEVICE_H

#include <exception>
#include <string>
#include <iostream>

#include "opencv2/opencv.hpp"

class CameraDevice
{
private:
    cv::VideoCapture* _cap;
    std::string _cam_addr;
    int _cam_idx;
    int _brightness;
    int _contrast;
    int _saturation;
    int _exposure;
    int _fps;

protected:

public:
    // Constructor / Destructor
    CameraDevice(std::string cam_addr);
    ~CameraDevice();

    // Functions
    std::string getAvailParams(); // Return as a string the available params: brightness, contrast etc
    void setParam(); // brightness, constrast, fps, etc
    //std::bool isOpened() { return this->_cap.isOpened(); }
    cv::Mat getFrame();

};


#endif /* CAMERA_DEVICE_H */
