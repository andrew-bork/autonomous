#include <subsystem/motor_driver.hpp>


#define THROTTLE_MIN 1000 // throttle minimum pwm 1 ms
#define THROTTLE_MAX 2000 // throttle maximum pwm 2 ms


subsystem::motor_driver::motor_driver(pca9685& _pca) : pca(_pca) {

}

void subsystem::motor_driver::set(motors motor, double power) {
    throttles[motor] = power;
    pca.set_pwm_ms(motor, (int) ((THROTTLE_MAX - THROTTLE_MIN) * power + THROTTLE_MIN));
}

void subsystem::motor_driver::set_all(double power) {
    throttles[0] = throttles[1] = throttles[2] = throttles[3] = power;
    pca.set_pwm_ms(motors::fl, (int) ((THROTTLE_MAX - THROTTLE_MIN) * power + THROTTLE_MIN));
    pca.set_pwm_ms(motors::fr, (int) ((THROTTLE_MAX - THROTTLE_MIN) * power + THROTTLE_MIN));
    pca.set_pwm_ms(motors::bl, (int) ((THROTTLE_MAX - THROTTLE_MIN) * power + THROTTLE_MIN));
    pca.set_pwm_ms(motors::br, (int) ((THROTTLE_MAX - THROTTLE_MIN) * power + THROTTLE_MIN));
}

void subsystem::motor_driver::use_mission_control(mission_control& control) {

    control.bind_readable("motors.fl", throttles[motors::fl]);
    control.bind_readable("motors.fr", throttles[motors::fr]);
    control.bind_readable("motors.bl", throttles[motors::bl]);
    control.bind_readable("motors.br", throttles[motors::br]);
}