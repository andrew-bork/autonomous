#include <mqtt/client.h>
#include <chrono>
#include <math/math.h>
#include <algorithm>

#include <util/str_utils.h>

#include <cstdlib>

#include <cstdio>
#include <ctime>
#include <csignal>

#include <util/log.h>


#include <extern/cxxopts.hpp>

#define MQTT_BROKER "localhost:1883"
#define MQTT_ID "guidance"

mqtt::client client(MQTT_BROKER, MQTT_ID, mqtt::create_options(MQTTVERSION_5));

// mqtt::message_ptr raw_mpu6050_data_msg_ptr = mqtt::make_message("adiru/raw-mpu6050_data", "");
// mqtt::message_ptr filtered_mpu6050_data_msg_ptr = mqtt::make_message("adiru/filtered-mpu6050_data", "");
// mqtt::message_ptr attitude_data_msg_ptr = mqtt::make_message("adiru/attitude", "");
mqtt::message_ptr motor_control_msg_ptr = mqtt::make_message("motors/speed", "");

double com_correction_factor = 0.5;


void set_thrust(double thrust) {
    std::string a = std::to_string(thrust);
    std::string b = std::to_string(thrust * (1 - com_correction_factor) / com_correction_factor );
    motor_control_msg_ptr->set_payload(a+" "+a+" "+b+" "+b);
    client.publish(motor_control_msg_ptr);
}

void sigint_handle(int s){
    set_thrust(0);
    std::quick_exit(0);
}

void process_message_string(std::string& message_string, math::vector& euler, math::vector& euler_rate, math::vector& acceleration) {
    string::tokenizer tokenizer(message_string, ' ');
    
    acceleration.x = std::stod(tokenizer.next_token());
    acceleration.y = std::stod(tokenizer.next_token());
    acceleration.z = std::stod(tokenizer.next_token());

    euler.z = std::stod(tokenizer.next_token());
    euler.y = std::stod(tokenizer.next_token());
    euler.z = std::stod(tokenizer.next_token());

    euler_rate.z = std::stod(tokenizer.next_token());
    euler_rate.y = std::stod(tokenizer.next_token());
    euler_rate.z = std::stod(tokenizer.next_token());
}

auto forever = std::chrono::seconds(std::numeric_limits<int>::max());
auto sec2 = std::chrono::seconds(2);

int main(int argc, char ** argv) {
    client.connect();

    client.subscribe("adiru/full");
    logger::bind_client(client);

    signal(SIGINT, sigint_handle);

    
    cxxopts::Options options("Auto Center of Mass", "dw abt it");
    options.add_options()
        ("m,maxthrust", "Maximum thrust allowed", cxxopts::value<double>()->default_value("0.3"))
        ("t,interval", "The interval between ramp ups", cxxopts::value<int>()->default_value("5"))
        ("r,rampthrust", "The amount of thrust to ramp up by", cxxopts::value<double>()->default_value("0.01"));

    options.parse_positional({"maxthrust"});
    auto result = options.parse(argc, argv);
    
    double max_thrust = result["maxthrust"].as<double>();



    bool running = true;
    math::vector euler, euler_rate;
    math::vector acceleration;

    double com_correction_min = 0.0;
    double com_correction_max = 1.0;

    double angle_confirm = 2 * DEG_TO_RAD;
    double angle_rest = 1 * DEG_TO_RAD;

    double thrust = 0.0;
    double ramp_up_thrust = result["rampthrust"].as<double>();
    // double max_thrust = 0.;

    int ramp_up_interval = result["interval"].as<int>();


    LOGD("Max Thrust: {:f}, Ramp-up Thrust: {:f}, Ramp-up Interval: {:d} s", max_thrust, ramp_up_thrust, ramp_up_interval);

    for(int i = 0; i < 7; i ++) {
        
        com_correction_factor = (com_correction_max + com_correction_min) / 2;
        LOGD("Iteration {:d} - Factor: {:f}", i, com_correction_factor);

        time_t start = time(NULL);
        int next = ramp_up_interval;
        thrust = 0.14;
        set_thrust(thrust);

        while (running)
        {
            // Construct a message pointer to hold an incoming message.
            mqtt::const_message_ptr message_pointer;
            bool running = client.try_consume_message_for(&message_pointer, forever);
            // LOGD("fdsafdsn");
            if (running) {
                // Construct a string from the message payload.
                std::string message_string = message_pointer -> get_payload_str();
                process_message_string(message_string, euler, euler_rate, acceleration);
                
                // LOGD("%s", message_string.c_str());
                // LOGD("%5.2f, %5.2f, %5.2f", euler.x * RAD_TO_DEG, euler.y * RAD_TO_DEG, euler.z * RAD_TO_DEG);

                if(euler.y > angle_confirm) {
                    LOGD("factor {:f} is too big", com_correction_factor);
                    com_correction_max = com_correction_factor;
                    break;
                }else if(euler.y < -angle_confirm) {
                    LOGD("factor {:f} is too small", com_correction_factor);
                    com_correction_min = com_correction_factor;
                    break;
                }

                time_t endt = time(NULL);
                if((endt - start) > next && thrust < max_thrust) {
                    next += ramp_up_interval;
                    thrust += ramp_up_thrust;
                    set_thrust(thrust);
                    LOGD("Ramping thrust up to {:f} %", thrust);
                }

            }
        }

        set_thrust(0.14);

        while (running)
        {
            // Construct a message pointer to hold an incoming message.
            mqtt::const_message_ptr message_pointer;
            bool running = client.try_consume_message_for(&message_pointer, forever);
            // LOGD("fdsafdsn");
            if (running) {
                // Construct a string from the message payload.
                std::string message_string = message_pointer -> get_payload_str();
                process_message_string(message_string, euler, euler_rate, acceleration);
                
                if(euler.y < angle_rest && euler.y > - angle_rest) {
                    LOGD("drone has returned to rest");
                    break;
                }

                // LOGD("%5.2f, %5.2f, %5.2f", euler.x * RAD_TO_DEG, euler.y * RAD_TO_DEG, euler.z * RAD_TO_DEG);
            }
        }
        std::this_thread::sleep_for(sec2);
    }

    LOGD("Result: {:f}", com_correction_factor);
}