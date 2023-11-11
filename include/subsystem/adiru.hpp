#pragma once

#include <math/vector.h>
#include <math/quarternion.h>
#include <math/filter.h>
#include <math/filter_vector.h>

#include <backend/mpu6050.h>
#include <backend/bmp390.h>
#include <missioncontrol.h>
// #include <backend/qmc5883.h>
// #include <backend/gps.h>

namespace subsystem {
    struct adiru {
        struct adiru_settings {
            double tau = 0.5; 
            double accelerometer_lowpass_cutoff = 5.0;
            double gyroscope_lowpass_cutoff = 5.0;
            double sample_rate = 60.0;
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
            
            filter::filter_vector accelerometer_filter, gyroscope_filter;

            math::vector accelerometer_acceleration, gyroscope_angular_velocity;
            math::vector raw_accelerometer_acceleration, raw_gyroscope_angular_velocity;

            math::vector position, orientation_euler;
            math::quarternion orientation = math::quarternion(1, 0, 0, 0);

    };
};