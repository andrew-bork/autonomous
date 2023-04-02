#include <util/timer.h>
#include <chrono>
// #include <logger.h>

void timer::create_timer(std::function<void(void)> command, unsigned int interval_ms) {
    auto now = std::chrono::steady_clock::now();
    auto then = std::chrono::steady_clock::now();
    double dt;
    while(1) {
        now = std::chrono::steady_clock::now();
        dt = std::chrono::duration_cast<std::chrono::nanoseconds> (now - then).count() * 0.000000001;
        then = now;

        auto next = std::chrono::steady_clock::now() + std::chrono::milliseconds(interval_ms);
        command();
        std::this_thread::sleep_until(next);
    }
}

void timer::create_timer(std::function<bool(void)> command, unsigned int interval_ms) {
    auto now = std::chrono::steady_clock::now();
    auto then = std::chrono::steady_clock::now();
    double dt;
    bool running = true;
    while(running) {
        now = std::chrono::steady_clock::now();
        dt = std::chrono::duration_cast<std::chrono::nanoseconds> (now - then).count() * 0.000000001;
        then = now;

        auto next = std::chrono::steady_clock::now() + std::chrono::milliseconds(interval_ms);
        running = command();
        std::this_thread::sleep_until(next);
    }
}
void timer::create_timer(std::function<void(double)> command, unsigned int interval_ms) {
    auto now = std::chrono::steady_clock::now();
    auto then = std::chrono::steady_clock::now();
    double dt = 0.0;
    while(1) {
        now = std::chrono::steady_clock::now();
        dt = std::chrono::duration_cast<std::chrono::nanoseconds> (now - then).count() * 0.000000001;
        then = now;

        auto next = std::chrono::steady_clock::now() + std::chrono::milliseconds(interval_ms);
        command(dt);
        // while(next < std::chrono::steady_clock::now()){
        //     next += chrono_interval;
        // }
        std::this_thread::sleep_until(next);
    }
}

void timer::create_timer(std::function<bool(double)> command, unsigned int interval_ms) {
    auto now = std::chrono::steady_clock::now();
    auto then = std::chrono::steady_clock::now();
    double dt;
    bool running = true;
    while(running) {
        now = std::chrono::steady_clock::now();
        dt = std::chrono::duration_cast<std::chrono::nanoseconds> (now - then).count() * 0.000000001;
        then = now;

        auto next = std::chrono::steady_clock::now() + std::chrono::milliseconds(interval_ms);
        running = command(dt);
        std::this_thread::sleep_until(next);
    }
}


// static void bruh(timer * t/* std::function<void(void)> command, bool running, unsigned int interval */){
//     auto now = std::chrono::steady_clock::now();
//     auto then = std::chrono::steady_clock::now();
//     while(t->running) {
//         now = std::chrono::steady_clock::now();
//         t->dt = std::chrono::duration_cast<std::chrono::nanoseconds> (now - then).count() * 0.000000001;
//         then = now;

//         std::lock_guard <std::mutex> lock(t->thread_mutex);
//         auto next = std::chrono::steady_clock::now() + std::chrono::milliseconds(t->interval);
//         t->command();
//         // while(next < std::chrono::steady_clock::now()){
//         //     next += chrono_interval;
//         // }
//         std::this_thread::sleep_until(next);
//     }
// }