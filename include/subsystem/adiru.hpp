#pragma once

#include <math/vector.h>
#include <math/quarternion.h>
#include <math/filter.h>

#include <backend/mpu6050.h>
#include <backend/bmp390.h>
#include <missioncontrol.h>
// #include <backend/qmc5883.h>
// #include <backend/gps.h>

namespace subsystem {
    struct adiru {
        struct adiru_settings {
            double tau = 0.5; 
        };
        adiru(adiru_settings _settings, mpu6050& _mpu, bmp390& _bmp);
        
        struct adiru_reading {
            math::vector position, orientation_euler;
            math::quarternion orientation = math::quarternion(1, 0, 0, 0);
        };

        adiru_reading update(double dt);

        void use_mission_control(mission_control& control);

        
        private:
            adiru_settings settings;
            mpu6050& mpu;
            bmp390& bmp;
            
            math::vector accelerometer_acceleration, gyroscope_angular_velocity;

            math::vector position, orientation_euler;
            math::quarternion orientation = math::quarternion(1, 0, 0, 0);

    };
};