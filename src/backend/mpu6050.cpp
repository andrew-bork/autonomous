#include "backend/mpu6050.h"


#include <iostream>
#include <unistd.h>
#include <time.h>

#include <cmath>
#include <thread>
#include <cstdio>
#include <string>
#include <cstdint>

#define DEG_TO_RAD 0.017453292519943
#define RAD_TO_DEG 57.29577951308232
#define SIGNED_16_BIT_MAX 0x7fff

/**
 * @brief Registers
 * 
 */

#define REG_PWR_MNG_1 0x6B // Register map 4.28
#define REG_PWR_MNG_2 0x6C // Register map 4.29
#define REG_CFG 0x1A
#define REG_GYRO_CFG 0x1B
#define REG_ACCL_CFG 0x1C
#define REG_FIFO_EN 0x23
#define REG_ACCL_OUT_STRT 0x3B
#define REG_TEMP_OUT_STRT 0x41
#define REG_GYRO_OUT_STRT 0x43
#define REG_SIG_PTH_RST 0x68 //Signal Path Reset
#define REG_USR_CTRL 0x6A

#define OUT_XACCL_H 0x3B
#define OUT_XACCL_L 0x3C
#define OUT_YACCL_H 0x3D
#define OUT_YACCL_L 0x3E
#define OUT_ZACCL_H 0x3F
#define OUT_ZACCL_L 0x40
#define OUT_TEMP_H 0x41
#define OUT_TEMP_L 0x42
#define OUT_XGYRO_H 0x43
#define OUT_XGYRO_L 0x44
#define OUT_YGYRO_H 0x45
#define OUT_YGYRO_L 0x46
#define OUT_ZGYRO_H 0x47
#define OUT_ZGYRO_L 0x48


#define read(r) (uint16_t) device.read_byte(r)
#define write(r,v) device.write_byte(r,v)

mpu6050::mpu6050(int addr) : device(addr){
	offsets[0] = 0; // X_ACCL_SHIFT;
	offsets[1] = 0; // Y_ACCL_SHIFT;
	offsets[2] = 0; // Z_ACCL_SHIFT;
	offsets[3] = 0; // X_GYRO_SHIFT;
	offsets[4] = 0; // Y_GYRO_SHIFT;
	offsets[5] = 0; // Z_GYRO_SHIFT;
}

static int offsets[6];

void mpu6050::print_debug(){

}

bool mpu6050::is_awake() {
	return (read(REG_PWR_MNG_1) & (0b01100000)) == 0b000000000;
}

void mpu6050::wake_up(){
	write(REG_PWR_MNG_1, read(REG_PWR_MNG_1) & (~0b01000000) ); // Clear the sleep bit to wake up

}

void mpu6050::sleep(){
	write(REG_PWR_MNG_1, read(REG_PWR_MNG_1) | 0b01000000);
}

void mpu6050::set_accl_set(accl_range range){
	switch(range){
		case g_16:
			accel_scale = 16 * 9.81 / SIGNED_16_BIT_MAX;
			break;
		case g_8: 
			accel_scale = 8 * 9.81 / SIGNED_16_BIT_MAX;
			break;
		case g_4: 
			accel_scale = 4 * 9.81 / SIGNED_16_BIT_MAX;
			break;
		case g_2: 
			accel_scale = 2 * 9.81 / SIGNED_16_BIT_MAX;
			break;
	}
	write(REG_ACCL_CFG, read(REG_ACCL_CFG) & (~0b00011000) | (range << 3));
}

void mpu6050::set_gyro_set(gyro_range range){
	switch(range){
		case deg_250:
			gyro_scale = 250 * DEG_TO_RAD / SIGNED_16_BIT_MAX;
			break;
		case deg_500:
			gyro_scale = 500 * DEG_TO_RAD / SIGNED_16_BIT_MAX;
			break;
		case deg_1000:
			gyro_scale = 1000 * DEG_TO_RAD / SIGNED_16_BIT_MAX;
			break;
		case deg_2000:
			gyro_scale = 2000 * DEG_TO_RAD / SIGNED_16_BIT_MAX;
			break;
	}
	write(REG_GYRO_CFG, read(REG_GYRO_CFG) & (~0b00011000) | (range << 3));


}

void mpu6050::set_clk(clk set){
	write(REG_PWR_MNG_1, read(REG_PWR_MNG_1) & (~0b00000111) | set);
}

void mpu6050::set_dlpf_bandwidth(dlpf set){
	write(REG_CFG, read(REG_CFG) & (~0b00000111) | set);
}

void mpu6050::set_fsync(fsync set){
	write(REG_CFG, read(REG_CFG) & (~0b00111000) | (set << 3));
}

int16_t handle_neg(int n){
	return (int16_t) n;
}

int16_t combine(uint8_t h, uint8_t l) {
	return (((uint16_t) h) << 8) | l; 
}

void mpu6050::get_data_raw(int * data){
	uint8_t buf[14];
	device.read_burst(OUT_XACCL_H, buf, 14);
	data[0] = combine(buf[0],buf[1]);
	data[1] = combine(buf[2],buf[3]);
	data[2] = combine(buf[4],buf[5]);
	data[3] = combine(buf[8],buf[9]);
	data[4] = combine(buf[10],buf[11]);
	data[5] = combine(buf[12],buf[13]);
}

void mpu6050::get_data_wo_offsets(double * data){
	uint8_t buf[14]; // 0-5 Accelerometer 
					// 6-7 Temp 
					// 8-13 Gyro 
	
	device.read_burst(OUT_XACCL_H, buf, 14); // All registers are in order. Just burst read them all.

	// Combine, convert to signed, and scale.
	data[0] = (((double) combine(buf[0], buf[1]))) * accel_scale;
	data[1] = (((double) combine(buf[2], buf[3]))) * accel_scale;
	data[2] = (((double) combine(buf[4], buf[5]))) * accel_scale;

	data[3] = (((double) combine(buf[8], buf[9]))) * gyro_scale;
	data[4] = (((double) combine(buf[10], buf[11]))) * gyro_scale;
	data[5] = (((double) combine(buf[12], buf[13]))) * gyro_scale;
}


void mpu6050::get_data(double * data){
	std::uint8_t buf[14]; // 0-5 Accelerometer 
					// 6-7 Temp 
					// 8-13 Gyro 
	
	device.read_burst(OUT_XACCL_H, buf, 14); // All registers are in order. Just burst read them all.

	// Combine, convert to signed, and scale.
	data[0] = (((double) combine(buf[0], buf[1]))) * accel_scale + offsets[0];
	data[1] = (((double) combine(buf[2], buf[3]))) * accel_scale + offsets[1];
	data[2] = (((double) combine(buf[4], buf[5]))) * accel_scale + offsets[2];

	data[3] = (((double) combine(buf[8], buf[9]))) * gyro_scale + offsets[3];
	data[4] = (((double) combine(buf[10], buf[11]))) * gyro_scale + offsets[4];
	data[5] = (((double) combine(buf[12], buf[13]))) * gyro_scale + offsets[5];
}

void mpu6050::get_data(math::vector& acceleration, math::vector& angular_velocity) {
	double data[6];
	get_data(data);
	acceleration = math::vector(data[0], data[1], data[2]);
	angular_velocity = math::vector(data[3], data[4], data[5]);
}

int mpu6050::query_register(int reg){
	return read(reg);
}

void mpu6050::set_register(int reg, int data){
	write(reg,data);
}


void mpu6050::calibrate(int n){
	double data[6];
	double error_sum[6];
	double kP[6] = {0.3, 0.3, 0.3, 0.3, 0.3, 0.3};
	double kI[6] = {90, 90, 90, 20, 20, 20};
	double expect[6] = {0, 0, 9.81, 0, 0, 0};
	for(int i = 0; i < 6; i++){
		offsets[i] = 0;
		error_sum[i] = 0;
	}

	for(int i = 0; i < n; i ++){
		for(int j = 0; j < 100; j ++){
			get_data(data);
			
			double dt = 0.001;
			for(int k = 0; k < 6; k ++){
				double error = expect[k] - (data[k] - offsets[k]);
				double p_term = error * kP[k];
				error_sum[k] += dt * error * kI[k];

				offsets[k] -= round((p_term + error_sum[k]) / 4);
			}

			usleep(1000);
		}
		
		for(int j = 0; j < 6; j++){
			kP[j] *= 0.75;
			kI[j] *= 0.75;
			error_sum[j] = 0;
		}
	}
}

mpu6050::~mpu6050() {
	device.close();
}

void mpu6050::set_offsets(double x_a, double y_a, double z_a, double x_g, double y_g, double z_g){
	offsets[0] = x_a;
	offsets[1] = y_a;
	offsets[2] = z_a;
	offsets[3] = x_g;
	offsets[4] = y_g;
	offsets[5] = z_g;
}

