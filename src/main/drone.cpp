/**
 * @file drone.cpp
 * @brief a program to update mission control with orientation information from the drone.
 * @version 0.1
 * @date 2023-09-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include <string>
#include <unistd.h>
#include <cmath> 

#include <math/quarternion.h>
#include <math/vector.h>
#include <math/constants.h>


#define delay(s) usleep(s * 1000 * 1000)
#define delay_ms(s) usleep(s * 1000)

namespace serialize
{
    /**
     * @brief Used to serialize vectors to send using libmissioncontrol
     * 
     * @param v 
     * @return std::string 
     */
    std::string serialize(const math::vector& v) {
        return "{\"x\":"+std::to_string(v.x)+",\"y\":"+std::to_string(v.y)+",\"z\":"+std::to_string(v.z)+"}";
    }
} // namespace serialize

#include <backend/mpu6050.h>

#include <missioncontrol.h>


void print_action(const char * msg) {
    printf("%s", msg);
    fflush(stdout);
}
void print_success(const char * msg = "OK") {
    printf("\x1b[1;32m%s\x1b[0m\n", msg);
} 
void print_fail(const char * msg = "FAILED") {
    printf("\x1b[1;31m%s\x1b[0m\n", msg);
}

mission_control control;

/**
 * @brief The readable information
 * 
 */
readable<math::vector> accelerometer_acceleration("accelerometer.acceleration", math::vector());
readable<math::vector> gyroscope_angular_velocity("gyroscope.angular_velocity", math::vector());

readable<std::string> status("status", "spooling up");

readable<math::vector> orientation_euler("orientation", math::vector());
math::quarternion orientation(1, 0, 0, 0);


double update_rate = 10;
int delay_ms = (int) (1000 / update_rate);
double dt = 1 / update_rate;
double tau = 0.5;


void setup() {
    /**
     * @brief This server will be on a tcp scoket at drone:3000
     * 
     */
    control.connect(3000);

    /**
     * @brief Test readable
     * 
     */
    control.bind_readable(accelerometer_acceleration);
    control.bind_readable(gyroscope_angular_velocity);
    control.bind_readable(orientation_euler);
    control.bind_readable(status);

    
    /**
     * @brief Test writable
     * 
     */
    control.add_writable<double>("update_rate", update_rate, [](double& new_update_rate) -> double {
        if(new_update_rate >= 1) {
            return new_update_rate;
        }
        return update_rate;
    });

    /**
     * @brief Ping! Pong!
     * prints out Pong! when the ping command is sent
     * 
     */
    control.add_command("ping", [&](std::vector<std::string> args) {
        printf("Pong!\n");
    });

    /**
     * @brief Commands can edit from different scopes. Ensure the lifetime of the variable.
     * 
     */
    control.add_command("bananify", [&](std::vector<std::string> args) {
        printf("BANANAED!\n");
        control.log("bananananan!!!!");
        status = "banananana";
    });

    /**
     * @brief Advertise the avaiable readables and commands
     * 
     */
    control.advertise();
    /**
     * @brief Update the mission control with all the bound readables.
     * 
     */
    control.tick();
    status = "initializing";
    control.tick();
    {
        print_action("Configuring MPU6050 ... ");
        mpu6050::init();
        bool calibrate = true;
        if(mpu6050::is_awake()) {
            calibrate = false;
        }
        mpu6050::set_accl_set(mpu6050::accl_range::g_2);
        mpu6050::set_gyro_set(mpu6050::gyro_range::deg_250);
        mpu6050::set_clk(mpu6050::clk::y_gyro);
        mpu6050::set_fsync(mpu6050::fsync::input_dis);
        mpu6050::set_dlpf_bandwidth(mpu6050::dlpf::hz_5);
        mpu6050::wake_up();

        mpu6050::calibrate(7);
        // if(calibrate) mpu6050::calibrate(7);
        print_success("DONE");
    }

    printf("Drone is ready\n");
    status = "ready";
}

int i = 0;


math::vector calculate_roll_pitch(const math::vector& acceleration) {
    math::vector euler;
    euler.x = atan2(acceleration.y, acceleration.z);
    euler.y = atan2(-acceleration.x, sqrt(acceleration.y * acceleration.y + acceleration.z * acceleration.z));
    return euler;
}


void loop() {
    // printf("tick %d\n", i++);
    mpu6050::read(*accelerometer_acceleration, *gyroscope_angular_velocity);


    double a_mag = math::length(accelerometer_acceleration);


    // Integrate angular velocity reading.
    math::quarternion gyro_quarternion = math::quarternion::from_euler_ZYX((*gyroscope_angular_velocity)*dt*DEG_TO_RAD);
    orientation = gyro_quarternion * orientation;
    
    orientation_euler = math::quarternion::to_euler(orientation);
    math::vector accelerometer_orientation;

    if(abs(a_mag - 1) < 0.1) {
        // If the acceleration seems in range, use it to calculate orientation.
        accelerometer_orientation = calculate_roll_pitch(accelerometer_acceleration);

        // Apply a complementary filter
        orientation_euler = accelerometer_orientation * tau + (*orientation_euler) * (1 - tau);

        // Convert the fused angles back into the quarternion.
        orientation = math::quarternion::from_euler_ZYX(orientation_euler);
    }

    control.tick();
    delay_ms(update_rate);
}

int main(){
    setup();
    while(1) loop();
}