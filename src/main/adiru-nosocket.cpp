#include <util/server.h>
#include <util/timer.h>
#include <math/filter.h>
#include <backend/mpu6050.h>
#include <backend/bmp390.h>
#include <backend/qmc5883.h>
#include <backend/gps.h>
#include <math/math.h>
#include <cmath>

#include <fcntl.h>
#include <unistd.h>

#include <thread>

void print_action(const char * msg) {
    printf("%s", msg);
    fflush(stdout);
}
void print_success(const char * msg = "OK") {
    printf("\x1b[1;32m%s\x1b[0m\n", msg);
} 
void print_fail(const char * msg = "FAILED") {
    printf("\x1b[1;31m%s\x1b[0m\n", msg);
}

broadcast_server server;

double ups = 60;

double data[32];

double * mpu6050_data = data; // 0 - 5
double * filtered_mpu6050_data = data + 6; // 6 - 11
double * euler_angles = data + 12; // 12 - 14

double * bmp390_data = data + 15; // 15 - 17

double * qmc5883_data = data + 18; // 18 - 20
double * gps_data = data + 21; // 21 - 23

double * debug = data + 24; // 24

filter::filter filters[6];

math::quarternion orientation;
math::vector orientation_euler;
math::vector position(0, 0, 0), velocity(0, 0, 0);

double sensor_roll_pitch_tau = 0.05;


gps gps_unit;

void tick(double dt) {
    
    // Read sensor data.
    {
        mpu6050::read(mpu6050_data);
        mpu6050_data[4] *= -1;

        bmp390::get_data(bmp390_data);
    }
    
    // Filter sensor data.
    {
        for(int i = 0; i < 6; i ++) {
            filtered_mpu6050_data[i] = filters[i][mpu6050_data[i]];
        }
    }


    math::vector euler_v = math::vector(
        filtered_mpu6050_data[3] * dt * DEG_TO_RAD, 
        filtered_mpu6050_data[4] * dt * DEG_TO_RAD, 
        filtered_mpu6050_data[5] * dt * DEG_TO_RAD
    );
    
    math::quarternion euler_q = math::quarternion::fromEulerZYX(euler_v);
    orientation = euler_q * orientation;
    orientation_euler = math::quarternion::toEuler(orientation);


    double a_dist_from_one_sqrd = 
        mpu6050_data[0] * mpu6050_data[0] + 
        mpu6050_data[1] * mpu6050_data[1] + 
        mpu6050_data[2] * mpu6050_data[2] - 1;
    
    euler_angles[0] = atan2(mpu6050_data[1], mpu6050_data[2]);
    euler_angles[1] = atan2((mpu6050_data[0]) , sqrt(mpu6050_data[1] * mpu6050_data[1] + mpu6050_data[2] * mpu6050_data[2]));
    euler_angles[2] = 0;


    orientation_euler.x = orientation_euler.x * (1 - sensor_roll_pitch_tau) + euler_angles[0] * sensor_roll_pitch_tau;
    orientation_euler.y = orientation_euler.y * (1 - sensor_roll_pitch_tau) + euler_angles[1] * sensor_roll_pitch_tau;
    
    orientation = math::quarternion::fromEulerZYX(orientation_euler);
    orientation_euler = math::quarternion::toEuler(orientation);
    
    qmc5883::get_data(qmc5883_data);

    debug[0] = dt;

    if(gps_unit.new_data) {
        gps_data[0] = gps_unit.lat;
        gps_data[1] = gps_unit.lng;
        gps_data[2] = (gps_unit.fix ? 1 : -1);
        gps_unit.new_data = false;
    }

    // math::vector temp = velocity * dt;
    // position = position + temp;
    // position.z = position.z * sensor_z_tau + bmp390_mpu6050_data[2] * (1 - sensor_z_tau);
    // temp = math::vector(filtered_mpu6050_data[0]*dt*G, filtered_mpu6050_data[1]*dt*G, -filtered_mpu6050_data[2]*dt*G);
    // temp = math::quarternion::rotateVector(orientation, temp);
    // temp.z += G*dt;
    // velocity = velocity + temp;
    // velocity.z = vzfilter[velocity.z];
    // velocity.z = velocity.z * (1 - sensor_z_tau) + valt * sensor_z_tau;


    server.tick();
}

int main() {
    {
        print_action("Initializing socket ... ");
        server.init("/tmp/adiru.sock");
        server.data = (char *)data;
        server.data_size = sizeof(data);
        print_success("DONE");
    }

    {
        print_action("Configuring MPU6050 ... ");
        mpu6050::init();
        mpu6050::set_accl_set(mpu6050::accl_range::g_2);
        mpu6050::set_gyro_set(mpu6050::gyro_range::deg_250);
        mpu6050::set_clk(mpu6050::clk::y_gyro);
        mpu6050::set_fsync(mpu6050::fsync::input_dis);
        mpu6050::set_dlpf_bandwidth(mpu6050::dlpf::hz_5);
        mpu6050::wake_up();

        mpu6050::calibrate(7);
        print_success("DONE");
    }

    {
        print_action("Configuring BMP390 ... ");
        bmp390::init();
        bmp390::soft_reset();
        bmp390::set_oversample(bmp390::oversampling::STANDARD, bmp390::ULTRA_LOW_POWER);
        bmp390::set_iir_filter(bmp390::COEFF_3);
        bmp390::set_output_data_rate(bmp390::hz50);
        bmp390::set_enable(true, true);
        
        bmp390::set_enable_fifo(true, true);
        bmp390::set_fifo_stop_on_full(false);

        bmp390::set_pwr_mode(bmp390::NORMAL);
        print_success("DONE");
    }

    {
        print_action("Configuring qmc5883 ... ");
        qmc5883::init();
        qmc5883::set_control_register(qmc5883::continuous, qmc5883::hz200, qmc5883::g2, qmc5883::k512);
        print_success("DONE");
    }

    {
        print_action("Setting up GPS ... ");
        gps_unit.init();
        print_success("DONE");
    }

    {
        double low = 10;
        for(int i = 0; i < 6; i ++) {
            filters[i] = filter::low_pass(ups, low);
        }
    }

    strcpy((char*) data, "Hello world!\n");

    printf("ADIRU running...\n");
    // double dt = 1 / 60.0;

    unsigned int interval_ms = (unsigned int) (1000 / ups);
    timer::create_timer(tick, interval_ms);
}
