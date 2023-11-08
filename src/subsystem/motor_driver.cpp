#include <subsystem/motor_driver.hpp>


#define THROTTLE_MIN 1000 // throttle minimum pwm 1 ms
#define THROTTLE_MAX 2000 // throttle maximum pwm 2 ms


subsystem::motor_driver::motor_driver(pca9685& _pca) : pca(_pca) {

}

void subsystem::motor_driver::set(motors motor, double power) {
    pca.set_pwm_ms(motor, (int) ((THROTTLE_MAX - THROTTLE_MIN) * power + THROTTLE_MIN));
}

void subsystem::motor_driver::set_all(double power) {
    pca.set_pwm_ms(motors::fl, (int) ((THROTTLE_MAX - THROTTLE_MIN) * power + THROTTLE_MIN));
    pca.set_pwm_ms(motors::fr, (int) ((THROTTLE_MAX - THROTTLE_MIN) * power + THROTTLE_MIN));
    pca.set_pwm_ms(motors::bl, (int) ((THROTTLE_MAX - THROTTLE_MIN) * power + THROTTLE_MIN));
    pca.set_pwm_ms(motors::br, (int) ((THROTTLE_MAX - THROTTLE_MIN) * power + THROTTLE_MIN));
}

void subsystem::motor_driver::use_mission_control(mission_control& control) {
    
}