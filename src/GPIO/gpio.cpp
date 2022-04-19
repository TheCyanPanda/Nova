#include <GPIO/gpio.h>
#include <cmath>
#include <ratio>
#include <thread>


namespace IO
{

using namespace GPIO;

    // Constructor
    PWMInterface::PWMInterface(int gpio, int max_pulse, int min_pulse)
        : _gpio(gpio), _minPulse(min_pulse), _maxPulse(max_pulse)
    {

    }

    // Destructor
    PWMInterface::~PWMInterface()
    {

    }

    testServo::testServo(int gpio)
        : PWMInterface(gpio, 0, 0)
    {
        this->test();
    }

    int testServo::test()
    {
        PWMOut pwm(18, 192, 0);
        // for (int i = 0; i < 3; ++i)
        // {
        //     for (int p = 0; p < 100; ++p)
        //     {
        //         pwm.set_ratio(p);
        //         std::this_thread::sleep_for(std::chrono::milliseconds(10));
        //     }

        //     //std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        //     for (int p = 100; p > 0; --p)
        //     {
        //         pwm.set_ratio(p);
        //         std::this_thread::sleep_for(std::chrono::milliseconds(10));
        //     }
        // }

    
        //pwm.set_ratio(10); // clockwise very fast
        //pwm.set_ratio(3);

        //pwm.set_ratio(10);
        //std::this_thread::sleep_for(std::chrono::milliseconds(100));
        for (int i = 0; i < 100; i++){
            pwm.set_ratio(40); // clockwise
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
        //pwm.set_ratio(50); // anticlockwise fast
        //pwm.set_ratio(60); // anti clockwise slow
        //pwm.set_ratio(80); // anti clockwise extremely slow
        //std::this_thread::sleep_for(std::chrono::milliseconds(200));

        return 0;
    }


}