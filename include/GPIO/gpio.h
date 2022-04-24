#ifndef GPIO_H
#define GPIO_H
//#include <cppgpio.hpp>
#include <csignal>
#include <iostream>
#include <chrono>
#include <bcm2835.h>
#include <exception>


namespace GPIO
{
    class GPIOException
        : public std::exception
    {
    public:
        const std::string msg;

        GPIOException(const std::string& msg_) : msg(msg_) {
            std::cout << "GPIO::PWMInterface::Exception - " << this->msg << "\n";
        }

        ~GPIOException() {
            std::cout << "GPIO::PWMInterface::Exception" << "\n";
        }

        virtual const char* what() const throw () {
            std::cout << "GPIO::PWMInterface::Exception::what" << "\n";
            return this->msg.c_str();
        }
};

    class PWMInterface
    {
        // Builds on the BCM2835 C library, see https://www.airspayce.com/mikem/bcm2835
    private:

    protected:
        int _pin;            // Pin number
        int _maxPulse;       // Max PWM
        int _minPulse;       // Min PWM
        int _dutyCycle;      // current duty cycle
        int _angle;          // Last set value
        int _pulseTime;      // pulse time in microseconds
        volatile bool _enable;      // enable/disabled
        void _setPwm(const int pulse);

        void move(const int value);

    public:
        PWMInterface(int pin, int max_pulse, int min_puls, int pulse_time = 2000);
        virtual ~PWMInterface();

        virtual int test() = 0;

    };

    class ServoInterface
        : public PWMInterface
    {
    private:

    public:
        ServoInterface(int gpio_pin);
        int test();
        void moveMax();
        void moveMin();
        void moveMid();

    };

}

#endif /* GPIO_H */
