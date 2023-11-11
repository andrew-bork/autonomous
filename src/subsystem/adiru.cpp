#include <math/serialize.hpp>
#include <subsystem/adiru.hpp>
#include <math/constants.h>
#include <cmath>



subsystem::adiru::adiru(adiru_settings _settings, mpu6050& _mpu, bmp390& _bmp) : mpu(_mpu), bmp(_bmp) {
    settings = _settings;

    accelerometer_filter = filter::low_pass(settings.sample_rate, settings.accelerometer_lowpass_cutoff);
    gyroscope_filter = filter::low_pass(settings.sample_rate, settings.gyroscope_lowpass_cutoff);

}


static math::vector calculate_roll_pitch(const math::vector& acceleration) {
    math::vector euler;
    euler.x = atan2(acceleration.y, acceleration.z);
    euler.y = atan2(-acceleration.x, sqrt(acceleration.y * acceleration.y + acceleration.z * acceleration.z));
    return euler;
}

subsystem::adiru::adiru_reading subsystem::adiru::update(double dt) {
    mpu.get_data(raw_accelerometer_acceleration, raw_gyroscope_angular_velocity);

    accelerometer_acceleration = accelerometer_filter[raw_accelerometer_acceleration];
    gyroscope_angular_velocity = gyroscope_filter[raw_gyroscope_angular_velocity];

    double a_mag = math::length(accelerometer_acceleration);


    // Integrate angular velocity reading.
    math::quarternion gyro_quarternion = math::quarternion::from_euler_ZYX(gyroscope_angular_velocity*dt);
    orientation = gyro_quarternion * orientation;
    
    orientation_euler = math::quarternion::to_euler(orientation);
    math::vector accelerometer_orientation;

    if(abs(a_mag - 9.8) < 1) {
        // If the acceleration seems in range, use it to calculate orientation.
        accelerometer_orientation = calculate_roll_pitch(accelerometer_acceleration);
        accelerometer_orientation.z = orientation_euler.z;
        // Apply a complementary filter
        orientation_euler = accelerometer_orientation * settings.tau + orientation_euler * (1 - settings.tau);

        // Convert the fused angles back into the quarternion.
        orientation = math::quarternion::from_euler_ZYX(orientation_euler);
    }

    // printf("%5.2f %5.2f\n", orientation_euler.x, orientation_euler.y);

    adiru_reading reading;
    reading.position = position;
    reading.orientation = orientation;
    reading.orientation_euler = orientation_euler;
    return reading;
}

void subsystem::adiru::use_mission_control(mission_control& control) {
    control.bind_readable("adiru.mpu6050.acceleration", raw_accelerometer_acceleration);
    control.bind_readable("adiru.mpu6050.gyroscope", raw_gyroscope_angular_velocity);
    control.bind_readable("adiru.filtered.acceleration", accelerometer_acceleration);
    control.bind_readable("adiru.filtered.gyroscope", gyroscope_angular_velocity);
    control.bind_readable("adiru.position", position);
    control.bind_readable("adiru.orientation", orientation);
    control.bind_readable("adiru.orientation_euler", orientation_euler);
    control.bind_readable("adiru.settings.tau", settings.tau);

    // control.set("adiru.tau", settings.tau);

    // double* tau_ptr = &settings.tau;

    control.add_writable<double>("adiru.settings.tau", settings.tau, [&] (double& new_value) -> double {
        // if(new_value < 0 || new_value > 1.0) { return *tau_ptr; }
        if(new_value < 0 || new_value > 1.0) { return settings.tau; }
        return new_value; 
    });
}

