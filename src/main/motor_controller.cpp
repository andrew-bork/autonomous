#include <mqtt/client.h>
#include <chrono>

#include <backend/pca9685.h>
#include <extern/cxxopts.hpp>

#include <util/str_utils.h>

#include <cstdio>
#include <unistd.h>

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

#define MQTT_BROKER "localhost:1883"
#define MQTT_ID "motor-controller"

mqtt::client client(MQTT_BROKER, MQTT_ID, mqtt::create_options(MQTTVERSION_5));

// mqtt::message_ptr raw_mpu6050_data_msg_ptr = mqtt::make_message("adiru/raw-mpu6050_data", "");
// mqtt::message_ptr filtered_mpu6050_data_msg_ptr = mqtt::make _message("adiru/filtered-mpu6050_data", "");
// mqtt::message_ptr attitude_data_msg_ptr = mqtt::make_message("adiru/attitude", "");

auto forever = std::chrono::seconds(std::numeric_limits<int>::max());

pca9685 pca;

int main(int argc, char ** argv) {
    cxxopts::Options options("Motor Controller", "This system controls the motors and led powers.");
    options.add_options()
        ("arm", "Whether or not to arm the craft", cxxopts::value<bool>()->default_value("false"));
    
    
    options.parse_positional({"ups"});
    auto result = options.parse(argc, argv);

    bool should_arm = result["arm"].as<bool>();
    pca.set_frequency(50);
    pca.wake_up();
    if(should_arm) {
        printf("arming motors\n");
        
        pca.set_pwm_ms(0, THROTTLE_MIN);
        pca.set_pwm_ms(1, THROTTLE_MIN);
        pca.set_pwm_ms(2, THROTTLE_MIN);
        pca.set_pwm_ms(3, THROTTLE_MIN);
        usleep(2000000);
        pca.set_pwm_ms(0, THROTTLE_MAX);
        pca.set_pwm_ms(1, THROTTLE_MAX);
        pca.set_pwm_ms(2, THROTTLE_MAX);
        pca.set_pwm_ms(3, THROTTLE_MAX);
        usleep(2000000);
        pca.set_pwm_ms(0, THROTTLE_MIN);
        pca.set_pwm_ms(1, THROTTLE_MIN);
        pca.set_pwm_ms(2, THROTTLE_MIN);
        pca.set_pwm_ms(3, THROTTLE_MIN);
        usleep(1000000);
    }

    client.connect();
    
    client.subscribe("motors/speed");
    client.subscribe("leds/brightness");

    double throttles[4];

    printf("finished intialization\n");
    bool running = true;
    while (running)
    {
        // Construct a message pointer to hold an incoming message.
        mqtt::const_message_ptr message_pointer;
        bool running = client.try_consume_message_for(&message_pointer, forever);
        // printf("fdsafdsn\n");
        if (running) {
            // Construct a string from the message payload.
            std::string message_string = message_pointer -> get_payload_str();
            std::string topic_string = message_pointer -> get_topic();
            
            if(topic_string == "leds/brightness") {
                string::tokenizer tokenizer(message_string, ' ');

                int pin = std::stoi(tokenizer.next_token());
                double brightness = std::stod(tokenizer.next_token());

                pca.set_pwm_ms(pin, (int) (PWM_FULL * brightness));
                printf("led: pin %d brightness to %4.2f\n", pin, brightness);
            }else if(topic_string == "motors/speed") {
                printf("%s\n", message_string.c_str());

                string::tokenizer tokenizer(message_string, ' ');

                throttles[0] = std::stod(tokenizer.next_token());
                throttles[1] = std::stod(tokenizer.next_token());
                throttles[2] = std::stod(tokenizer.next_token());
                throttles[3] = std::stod(tokenizer.next_token());


                pca.set_pwm_ms(0, (int) ((THROTTLE_MAX - THROTTLE_MIN) * throttles[0] + THROTTLE_MIN));
                pca.set_pwm_ms(1, (int) ((THROTTLE_MAX - THROTTLE_MIN) * throttles[1] + THROTTLE_MIN));
                pca.set_pwm_ms(2, (int) ((THROTTLE_MAX - THROTTLE_MIN) * throttles[2] + THROTTLE_MIN));
                pca.set_pwm_ms(3, (int) ((THROTTLE_MAX - THROTTLE_MIN) * throttles[3] + THROTTLE_MIN));

                printf("throttles: %4.2f %, %4.2f %, %4.2f %, %4.2f %\n", throttles[0], throttles[1], throttles[2], throttles[3]);
            
            }
            
        }
    }
}