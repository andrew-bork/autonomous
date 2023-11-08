/**
 * @file drone.cpp
 * @brief a program to update mission control with orientation information from the drone.
 * @version 0.1
 * @date 2023-09-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <cstdlib>

#include <string>
#include <unistd.h>
#include <cmath> 


#include <math/quarternion.h>
#include <math/vector.h>
#include <math/constants.h>

#include <math/serialize.hpp>

#define delay(s) usleep(s * 1000 * 1000)
#define delay_ms(s) usleep(s * 1000)

#include <backend/mpu6050.h>

#include <subsystem/adiru.hpp>
#include <subsystem/led_driver.hpp>
#include <subsystem/motor_driver.hpp>

#include <util/print_utils.hpp>

#include <missioncontrol.h>


double update_rate = 10;
int delay_ms = (int) (1000 / update_rate);
double dt = 1 / update_rate;
struct {
    mpu6050* mpu = NULL;
    bmp390* bmp = NULL;
    pca9685* pca = NULL;

} peripherals;

struct {
    mission_control* control = NULL;
    subsystem::adiru* adiru = NULL;
    subsystem::led_driver* led_driver = NULL;
    subsystem::motor_driver* motor_driver = NULL;
} subsystems;

void setup() {
    
    static mission_control control;
    control.connect(3000);

    subsystems.control = &control;

    static mpu6050 mpu;

    {
        print_action("Configuring MPU6050 ... ");
        mpu.set_accl_set(mpu6050::accl_range::g_2);
        mpu.set_gyro_set(mpu6050::gyro_range::deg_250);
        mpu.set_clk(mpu6050::clk::y_gyro);
        mpu.set_fsync(mpu6050::fsync::input_dis);
        mpu.set_dlpf_bandwidth(mpu6050::dlpf::hz_5);
        mpu.wake_up();

        mpu.calibrate(7);
        print_success("DONE");
    }

    static bmp390 bmp;
    {
        print_action("Configuring BMP390 ... ");
        bmp.soft_reset();
        bmp.set_oversample(bmp390::oversampling::STANDARD, bmp390::ULTRA_LOW_POWER);
        bmp.set_iir_filter(bmp390::COEFF_3);
        bmp.set_output_data_rate(bmp390::hz50);
        bmp.set_enable(true, true);
        
        bmp.set_enable_fifo(false, false);
        bmp.set_fifo_stop_on_full(false);

        bmp.set_pwr_mode(bmp390::NORMAL);
        print_success("DONE");
    }

    static pca9685 pca;
    {
        print_action("Getting PCA9685 ... ");
        if(!pca.is_awake()) {
            print_fail();
            printf("pca wasn't initialized already. was it armed?\n");
            throw std::runtime_error("pca was not armed");
        }
        print_success("DONE");
    }

    peripherals.mpu = &mpu;
    peripherals.bmp = &bmp;
    peripherals.pca = &pca;

    subsystem::adiru::adiru_settings initial_adiru_settings;

    static subsystem::adiru adiru(initial_adiru_settings, mpu, bmp);
    adiru.use_mission_control(control);

    static subsystem::led_driver led_driver(pca);
    led_driver.use_mission_control(control);

    static subsystem::motor_driver motor_driver(pca);
    motor_driver.use_mission_control(control);

    subsystems.adiru = &adiru;
    subsystems.led_driver = &led_driver;
    subsystems.motor_driver = &motor_driver;

    control.advertise();


    printf("Drone is ready\n");
    subsystems.led_driver->set(subsystem::green, subsystem::high);
    // status = "ready";
}

int i = 0;


math::vector calculate_roll_pitch(const math::vector& acceleration) {
    math::vector euler;
    euler.x = atan2(acceleration.y, acceleration.z);
    euler.y = atan2(-acceleration.x, sqrt(acceleration.y * acceleration.y + acceleration.z * acceleration.z));
    return euler;
}


void loop() {

    auto adiru_reading = subsystems.adiru->update(dt);


    subsystems.control->tick();
    delay_ms(update_rate);
}

void set_stop_leds() {

    pca9685 pca;
    {
        if(!pca.is_awake()) {
            return;
        }
    }
    subsystem::led_driver led_driver(pca);

    led_driver.set(subsystem::red, subsystem::high);
    led_driver.set(subsystem::green, subsystem::low);
}

int main(){
    try {
        std::atexit(set_stop_leds);
        setup();
        while(1) loop();
    }catch(std::exception& e) {
        if(subsystems.led_driver != NULL) {
            subsystems.led_driver->set(subsystem::red, subsystem::high);
            subsystems.led_driver->set(subsystem::green, subsystem::low);
        }
    }
}