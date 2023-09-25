#include <util/timer.h>
#include <extern/cxxopts.hpp>
#include <cstdio>

#include <backend/mpu6050.h>
#include <math/filter.h>
#include <math/math.h>
#include <cmath>

#include <mqtt/client.h>

#define MQTT_BROKER "localhost:1883"
#define MQTT_ID "adiru"

double mpu6050_data[6];
double filtered_mpu6050_data[6];

filter::filter filters[6];

mqtt::client client(MQTT_BROKER, MQTT_ID, mqtt::create_options(MQTTVERSION_5));
mqtt::message_ptr raw_mpu6050_data_msg_ptr = mqtt::make_message("adiru/raw-mpu6050_data", "");
mqtt::message_ptr filtered_mpu6050_data_msg_ptr = mqtt::make_message("adiru/filtered-mpu6050_data", "");
mqtt::message_ptr attitude_data_msg_ptr = mqtt::make_message("adiru/attitude", "");
mqtt::message_ptr full_data_frame_msg_ptr = mqtt::make_message("adiru/full", "");

math::quarternion orientation;
math::vector orientation_euler;
math::vector position(0, 0, 0), velocity(0, 0, 0);

double sensor_roll_pitch_tau = 0.05;

void tick(double dt) {
    mpu6050::read(mpu6050_data);

    mpu6050_data[4] *= -1;
    
    for(int i = 0; i < 6; i ++) {
        filtered_mpu6050_data[i] = filters[i][mpu6050_data[i]];
    }
    
    math::vector euler_v = math::vector(filtered_mpu6050_data[3]*dt*DEG_TO_RAD, filtered_mpu6050_data[4]*dt*DEG_TO_RAD, filtered_mpu6050_data[5]*dt*DEG_TO_RAD);
    math::quarternion euler_q = math::quarternion::from_euler_ZYX(euler_v);
    orientation = euler_q*orientation;
    orientation_euler = math::quarternion::to_euler(orientation);


    double a_dist_from_one_sqrd = mpu6050_data[0] * mpu6050_data[0] + mpu6050_data[1] * mpu6050_data[1] + mpu6050_data[2] * mpu6050_data[2] - 1;
    double roll = atan2(mpu6050_data[1], mpu6050_data[2]);
    double pitch = atan2((mpu6050_data[0]) , sqrt(mpu6050_data[1] * mpu6050_data[1] + mpu6050_data[2] * mpu6050_data[2]));

    orientation_euler.x = orientation_euler.x * (1 - sensor_roll_pitch_tau) + roll * sensor_roll_pitch_tau;
    orientation_euler.y = orientation_euler.y * (1 - sensor_roll_pitch_tau) + pitch * sensor_roll_pitch_tau;
    
    orientation = math::quarternion::from_euler_ZYX(orientation_euler);
    orientation_euler = math::quarternion::to_euler(orientation);
    

    // math::vector temp = velocity * dt;
    // position = position + temp;
    // position.z = position.z * sensor_z_tau + bmp390_mpu6050_data[2] * (1 - sensor_z_tau);
    // temp = math::vector(filtered_mpu6050_data[0]*dt*G, filtered_mpu6050_data[1]*dt*G, -filtered_mpu6050_data[2]*dt*G);
    // temp = math::quarternion::rotate_vector(orientation, temp);
    // temp.z += G*dt;
    // velocity = velocity + temp;
    // velocity.z = vzfilter[velocity.z];
    // velocity.z = velocity.z * (1 - sensor_z_tau) + valt * sensor_z_tau;

    raw_mpu6050_data_msg_ptr->set_payload(
        std::to_string(mpu6050_data[0]) + " " + 
        std::to_string(mpu6050_data[1]) + " " + 
        std::to_string(mpu6050_data[2]) + " " + 
        std::to_string(mpu6050_data[3]) + " " + 
        std::to_string(mpu6050_data[4]) + " " + 
        std::to_string(mpu6050_data[5]));
    filtered_mpu6050_data_msg_ptr->set_payload(
        std::to_string(filtered_mpu6050_data[0]) + " " +
        std::to_string(filtered_mpu6050_data[1]) + " " + 
        std::to_string(filtered_mpu6050_data[2]) + " " + 
        std::to_string(filtered_mpu6050_data[3]) + " " + 
        std::to_string(filtered_mpu6050_data[4]) + " " + 
        std::to_string(filtered_mpu6050_data[5]));
    attitude_data_msg_ptr->set_payload(
        std::to_string(orientation_euler.x) + " " + 
        std::to_string(orientation_euler.y) + " " + 
        std::to_string(orientation_euler.z) + " " + 
        std::to_string(filtered_mpu6050_data[3]) + " " + 
        std::to_string(filtered_mpu6050_data[4]) + " " + 
        std::to_string(filtered_mpu6050_data[5]));
    full_data_frame_msg_ptr->set_payload(
        std::to_string(filtered_mpu6050_data[0]) + " " + 
        std::to_string(filtered_mpu6050_data[1]) + " " + 
        std::to_string(filtered_mpu6050_data[2]) + " " + 
        std::to_string(orientation_euler.x) + " " + 
        std::to_string(orientation_euler.y) + " " + 
        std::to_string(orientation_euler.z) + " " + 
        std::to_string(filtered_mpu6050_data[3]) + " " + 
        std::to_string(filtered_mpu6050_data[4]) + " " + 
        std::to_string(filtered_mpu6050_data[5]));


    client.publish(raw_mpu6050_data_msg_ptr);
    client.publish(filtered_mpu6050_data_msg_ptr);
    client.publish(attitude_data_msg_ptr);
    client.publish(full_data_frame_msg_ptr);
    // printf("tick (%f)\n", dt);
}

int main(int argc, char ** argv) {
    cxxopts::Options options("ADIRU", "Air mpu6050_data Inertial Reference Unit (ADIRU) provides position and attitude information to the craft.");
    options.add_options()
        ("ups", "The update rate for this unit", cxxopts::value<double>()->default_value("60"))
        ("l", "Low pass filter cutoff", cxxopts::value<double>()->default_value("10"));
    
    
    options.parse_positional({"ups"});
    auto result = options.parse(argc, argv);
    
    double ups = result["ups"].as<double>();
    int interval_ms = (int) (1000 / ups);


    {
        printf("Begin mpu6050 configuration\n");
        mpu6050::init();
        mpu6050::set_accl_set(mpu6050::accl_range::g_2);
        mpu6050::set_gyro_set(mpu6050::gyro_range::deg_250);
        mpu6050::set_clk(mpu6050::clk::y_gyro);
        mpu6050::set_fsync(mpu6050::fsync::input_dis);
        mpu6050::set_dlpf_bandwidth(mpu6050::dlpf::hz_5);
        mpu6050::wake_up();

        mpu6050::calibrate(7);
        printf("Finish mpu6050 configuration\n");
    }

    {
        printf("Starting up MQTT client\n");
        client.connect();

    }

    // double low = result["l,low"].as<double>();
    double low = 10;

    {
        for(int i = 0; i < 6; i ++) {
            filters[i] = filter::low_pass(ups, low);
        }
    }


    printf("Finished initialization\n");
    timer::create_timer(tick, interval_ms);
}