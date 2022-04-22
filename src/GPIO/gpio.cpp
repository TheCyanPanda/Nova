#include <GPIO/gpio.h>
#include <cmath>
#include <ratio>
#include <thread>


namespace GPIO
{

    // Constructor
    PWMInterface::PWMInterface(int pin, int max_pulse, int min_pulse, int pulse_time)
        : _pin(pin), _minPulse(min_pulse), _maxPulse(max_pulse), _pulseTime(pulse_time)
    {

        if (!bcm2835_init())
            throw GPIOException("ERROR: Could not initialize bcm2835 driver\n");
        std::cout << "init bcm2835" << "\n"; // todo: Store in log
        bcm2835_gpio_fsel(this->_pin, BCM2835_GPIO_FSEL_OUTP);  // Set GPIO pin as output
        this->_dutyCycle = 0;
    }

    // Destructor
    PWMInterface::~PWMInterface()
    {
        // Todo; disable the port ? bcm2835_gpio_fsel(this->_pin, ---);  // Set GPIO pin as output
    }

    void PWMInterface::move(const int angle)
    {
        this->_dutyCycle = angle;
        this->_angle = 0;
        this->_setPwm(20);
    }

    void PWMInterface::moveMax()
    {
        //int pulse = pulse; // Todo; find the max pulse value
        this->_dutyCycle = this->_maxPulse;
        this->_angle = 0;
        this->_setPwm(30);
    }

    void PWMInterface::moveMin()
    {
        //int pulse = pulse; // Todo; find the max pulse value
        this->_dutyCycle = this->_minPulse;
        this->_angle = 0;
        this->_setPwm(30);
    }

    void PWMInterface::moveMid()
    {
        //int pulse = pulse; // Todo; find the max pulse value

        this->_dutyCycle = this->_minPulse + (this->_maxPulse - this->_minPulse) / 2;
        this->_angle = 0;
        this->_setPwm(30);
    }

    void PWMInterface::_setPwm(const int pulse)
    {
        for (int j = 0; j < pulse; ++j)
        {
            bcm2835_gpio_set(this->_pin); // set high
            bcm2835_delayMicroseconds(this->_dutyCycle);
            bcm2835_gpio_clr(this->_pin);   //Set low
            bcm2835_delayMicroseconds(this->_pulseTime - this->_dutyCycle); //each pulse is 20ms
        }
        std::cout << "duty cycle: " << this->_dutyCycle << " pulse: " << pulse << "\n";
    }

    testServo::testServo(int gpio_pin)
        : PWMInterface(gpio_pin, 900, 2000, 20000)
    {
        this->test();
    }

    int testServo::test()
    {   
        this->moveMid();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        this->moveMin();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        this->moveMax();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        this->moveMid();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        this->moveMin();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        this->moveMax();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        this->moveMid();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        this->moveMin();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        this->moveMax();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        return 0;
    }


}