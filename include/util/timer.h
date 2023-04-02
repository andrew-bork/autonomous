#ifndef TIMER_H_GUARD
#define TIMER_H_GUARD

#include <functional>
#include <thread>
#include <mutex>
namespace timer {
    void create_timer(std::function<void(void)> command, unsigned int interval_ms);
    void create_timer(std::function<bool(void)> command, unsigned int interval_ms);
    void create_timer(std::function<void(double)> command, unsigned int interval_ms);
    void create_timer(std::function<bool(double)> command, unsigned int interval_ms);
    
    struct timer {
        bool running; // Change this to stop the timer
        unsigned int interval; // Time interval in ms 
        double dt;
        std::function<void(void)> command;
        
        timer();
        timer(std::function<void(void)> command, unsigned int interval_ms);
        ~timer();

        void start(std::function<void(void)> command, unsigned int interval_ms);
        void start();
        void stop();
    };

    struct threaded_timer {
        bool running; // Change this to stop the timer
        std::thread thread; // Underlying thread
        unsigned int interval; // Time interval in ms 
        double dt;
        std::function<void(void)> command;
        
        threaded_timer();
        threaded_timer(std::function<void(void)> command, unsigned int interval_ms);
        ~threaded_timer();

        void start(std::function<void(void)> command, unsigned int interval_ms);
        void start();
        void stop();

    };
}

// timer_thread create_timer(std::function<void(void)> command, unsigned int interval_ms);

#endif