#pragma once
#include <cstdint>
#include <backend/i2c.h>
struct pca9685 {
    public:
        pca9685();
        ~pca9685();

        

        void wake_up();
        bool is_awake();
        void restart();
        void sleep();

        void set_frequency(int rate);
        int get_frequency();

        void set_pwm_on(std::uint8_t pwm, std::uint16_t on);
        void set_pwm_off(std::uint8_t pwm, std::uint16_t off);

        void set_pwm_percent(std::uint8_t pwm, double percent);
        void set_pwm_ms(std::uint8_t pwm, std::uint16_t ms);


        // Set all pwms on time delays
        void set_all_pwm_on();
        // Set all pwms off time delays
        void set_all_pwm_off();

        // Set all pwms percent of full
        void set_all_pwm_percent();
        // Set all pwms percent of 
        void set_all_pwm_ms();

        int query_reg(std::uint8_t reg);
        void write_reg(std::uint8_t reg, std::uint8_t val);
    
    private:
        void get_frequency_from_device();
        int frequency = -1; // hz
        int period = -1; // ms
        i2c::device device;

};