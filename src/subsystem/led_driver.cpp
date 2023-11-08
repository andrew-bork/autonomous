

#define MOTOR_FL_PIN 0
#define MOTOR_FR_PIN 1
#define MOTOR_BL_PIN 2
#define MOTOR_BR_PIN 3

#define LED_RUN_PIN 15
#define LED_ERROR_PIN 14
#define LED_AUTO_PIN 11
#define LED_WHITE_PIN 13
#define LED_BLUE_PIN 12

#define THROTTLE_MIN 1000 // throttle minimum pwm 1 ms
#define THROTTLE_MAX 2000 // throttle maximum pwm 2 ms
#define PWM_FULL 19999

#include <subsystem/led_driver.hpp>

subsystem::led_driver::led_driver(pca9685& _pca) : pca(_pca) {

}


void subsystem::led_driver::set(subsystem::leds led, subsystem::led_value value) {
    if(value) {
        pca.set_pwm_ms(led, PWM_FULL);
    }else {
        pca.set_pwm_ms(led, 0);
    }
}
void subsystem::led_driver::set(subsystem::leds led, double brightness) {
    pca.set_pwm_ms(led, (int) (PWM_FULL * brightness));
}


void subsystem::led_driver::use_mission_control(mission_control& control) {
    
}