#pragma once
#include <cstdint>
#include <backend/pca9685.h>
#include <missioncontrol.h>


#define MOTOR_FL_PIN 0
#define MOTOR_FR_PIN 1
#define MOTOR_BL_PIN 2
#define MOTOR_BR_PIN 3

namespace subsystem {
    enum motors : std::uint8_t {
        fl = 0,
        fr = 1,
        bl = 2,
        br = 3,
    };


    struct motor_driver {
            motor_driver(pca9685& _pca);
            
            void set(motors motor, double power);
            void set_all(double power);

            void use_mission_control(mission_control& control);

        private:

            double throttles[4];

            pca9685& pca;
    };
}