#ifndef GPIO_H
#define GPIO_H
#include <cppgpio.hpp>
#include <csignal>
#include <iostream>
#include <chrono>


namespace IO
{
    class PWMInterface
    {

    private:

    protected:
        int _gpio;      // Pin number
        int _maxPulse;  // Max PWM
        int _minPulse;  // Min PWM
        int _pw;        // current PWM
        int _pwInc;     // PWM increment
        volatile bool _run;      // enable/disabled

    public:
        PWMInterface(int gpio, int max_pulse, int min_puls);
        virtual ~PWMInterface();

        virtual int test() = 0;

    };

    class testServo
        : public PWMInterface
    {
    private:

    public:
        testServo(int gpio);
        int test();
    };

}

#endif /* GPIO_H */
