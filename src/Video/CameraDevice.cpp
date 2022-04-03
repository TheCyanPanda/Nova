#include <Video/CameraDevice.h>

// Constructor / Destructor
CameraDevice::CameraDevice(std::string cam_addr)
    : _cam_addr(cam_addr)
{
    std::cout << "DEBUG: Initiating cam with addr: " << this->_cam_addr << "\n";
    //this->_cap = new cv::VideoCapture(this->_cam_addr);
    this->_cap = new cv::VideoCapture();
    this->_cam_idx = std::stoi(&this->_cam_addr.back());
    this->_cap->open(this->_cam_idx);
}

CameraDevice::~CameraDevice()
{
    delete this->_cap;
}


// Public functions
cv::Mat CameraDevice::getFrame()
{

    if (!this->_cap->isOpened())
	{
		throw std::runtime_error("Camera is not opened yet");
	}

    cv::Mat frame;
    *this->_cap >> frame;

    return frame;
}