#include <backend/qmc5883.h>
#include <cstdio>
#include <unistd.h>
#include <fstream>
#include <cmath>
#include <iostream>
#include <math/filter.h>
// #include <mqtt/client.h>

// #define MQTT_BROKER "localhost:1883"
// #define MQTT_ID "test"

// mqtt::client client(MQTT_BROKER, MQTT_ID, mqtt::create_options(MQTTVERSION_5));

// mqtt::message_ptr msg_ptr = mqtt::make_message("test/mag", "");

int main() {

    filter::filter filters[3];
    for(int i = 0; i < 3; i ++){
        filters[i] = filter::low_pass(100, 2);
    }

    std::ofstream out("mag-data.csv");

    qmc5883::init();
    qmc5883::set_control_register(qmc5883::continuous, qmc5883::hz200, qmc5883::g2, qmc5883::k64);

    double data[3];
    // int16_t data[3];
    while(1) {;
        // if(qmc5883::is_data_ready()) {
            // qmc5883::get_raw_data(data);
            qmc5883::get_data(data);

            for(int i = 0; i < 3; i ++){
                data[i] = filters[i][data[i]];
            }
            // double direction = atan2(data[1], data[0]) * 180 / M_PI;
            // printf("%7.3f\r", direction);
            // fflush(stdout);
            std::cout << data[0] << "," << data[1] << "," << data[2] << std::endl;

        // }
        usleep(1000000/100);
    }
}

