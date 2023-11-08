#include <subsystem/adiru.hpp>
#include <math/constants.h>
#include <cmath>


subsystem::adiru::adiru(adiru_settings _settings, mpu6050& _mpu, bmp390& _bmp) : mpu(_mpu), bmp(_bmp) {
    settings = _settings;
}


static math::vector calculate_roll_pitch(const math::vector& acceleration) {
    math::vector euler;
    euler.x = atan2(acceleration.y, acceleration.z);
    euler.y = atan2(-acceleration.x, sqrt(acceleration.y * acceleration.y + acceleration.z * acceleration.z));
    return euler;
}

subsystem::adiru::adiru_reading subsystem::adiru::update(double dt) {
    mpu.get_data(accelerometer_acceleration, gyroscope_angular_velocity);


    double a_mag = math::length(accelerometer_acceleration);


    // Integrate angular velocity reading.
    math::quarternion gyro_quarternion = math::quarternion::from_euler_ZYX(gyroscope_angular_velocity*dt*DEG_TO_RAD);
    orientation = gyro_quarternion * orientation;
    
    orientation_euler = math::quarternion::to_euler(orientation);
    math::vector accelerometer_orientation;

    if(abs(a_mag - 1) < 0.1) {
        // If the acceleration seems in range, use it to calculate orientation.
        accelerometer_orientation = calculate_roll_pitch(accelerometer_acceleration);

        // Apply a complementary filter
        orientation_euler = accelerometer_orientation * settings.tau + orientation_euler * (1 - settings.tau);

        // Convert the fused angles back into the quarternion.
        orientation = math::quarternion::from_euler_ZYX(orientation_euler);
    }

    adiru_reading reading;
    reading.position = position;
    reading.orientation = orientation;
    reading.orientation_euler = orientation_euler;
    return reading;
}

void subsystem::adiru::use_mission_control(mission_control& control) {

}

