#pragma once
#include <backend/pca9685.h>
#include <cstdint>
#include <missioncontrol.h>

namespace subsystem {

    enum leds : std::uint8_t {
        green = 15,
        red = 14,
        yellow = 11,
        white = 13,
        blue = 12
    };

    enum led_value : bool {
        high = true,
        low = false,
    };

    struct led_driver {
            led_driver(pca9685& _pca);

            void set(leds led, led_value value);
            void set(leds led, double brightness);
            void use_mission_control(mission_control& control);

        private: 
            double brightnesses[5];
            pca9685& pca;
    };

}