#include <backend/qmc5883.h>
#include <cstdio>
#include <unistd.h>
#include <mqtt/client.h>

#define MQTT_BROKER "localhost:1883"
#define MQTT_ID "test"

mqtt::client client(MQTT_BROKER, MQTT_ID, mqtt::create_options(MQTTVERSION_5));

mqtt::message_ptr msg_ptr = mqtt::make_message("test/mag", "");

int main() {
    client.connect();
    qmc5883::init();
    qmc5883::set_control_register(qmc5883::continuous, qmc5883::hz50, qmc5883::g2, qmc5883::k512);
    int16_t data[3];
    double data2[3];
    while(1) {
        qmc5883::get_raw_data(data);
        if(qmc5883::is_data_ready()) {
            data2[0] = data[0];
            data2[1] = data[1];
            data2[2] = data[2];
            // printf("%f,%f,%f\n", data2[0], data2[1], data2[2]);
            msg_ptr->set_payload(std::to_string(data2[0]) + " " + std::to_string(data2[1]) + " " + std::to_string(data2[2]));
            client.publish(msg_ptr);
            // fflush(stdout);
        }
        usleep(1000000/100);
    }
}

