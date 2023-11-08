#include <backend/pca9685.h>
#include <subsystem/led_driver.hpp>
#include <subsystem/motor_driver.hpp>
#include <stdexcept>
#include <util/print_utils.hpp>
#include <unistd.h>

int main() {
    static pca9685 pca;
    {
        print_action("Configuring PCA9685 ... ");
        pca.set_frequency(50);
        pca.wake_up();
        print_success("DONE");
    }

    static subsystem::led_driver led_driver(pca);
    static subsystem::motor_driver motor_driver(pca);

    led_driver.set(subsystem::green, subsystem::high);
    led_driver.set(subsystem::blue, subsystem::high);
    led_driver.set(subsystem::red, subsystem::high);
    led_driver.set(subsystem::yellow, subsystem::high);
    led_driver.set(subsystem::white, subsystem::high);

    
    print_action("Arming motors. STAND BACK! ... ");
    motor_driver.set_all(0.0);
    usleep(2000000);
    motor_driver.set_all(1.0);
    usleep(2000000);
    motor_driver.set_all(0.0);
    usleep(1000000);
    print_success("DONE");

    led_driver.set(subsystem::green, subsystem::low);
    led_driver.set(subsystem::blue, subsystem::low);
    led_driver.set(subsystem::red, subsystem::low);
    led_driver.set(subsystem::yellow, subsystem::low);
    led_driver.set(subsystem::white, subsystem::low);
}