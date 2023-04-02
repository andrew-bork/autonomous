//-------------------------------MPU6050 Accelerometer and Gyroscope C++ library-----------------------------
//Copyright (c) 2019, Alex Mous
//Licensed under the CC BY-NC SA 4.0

// MODIFIED BY ANDREW LIN

//Include the header file for this class
#include "backend/mpu6050.h"


#include <iostream>
#include <unistd.h>
#include <time.h>

#include <cmath>
#include <thread>
#include <cstdio>
#include <string>
#include <cstdint>

#include <backend/i2c.h>
// #include <logger.h>

#define Read(r) (uint16_t) mpu.read_byte(r)
#define Write(r,v) mpu.write_byte(r,v)

// static int fd;

void mpu6050::init(){init(MPU6050_DEFAULT_ADDR);}

static int offsets[6];

static int accl_scale = 16384;
static double gyro_scale = 16.4;

static i2c::device mpu;

void mpu6050::init(int addr){
	mpu = i2c::device(addr);
	
	offsets[0] = 0; // X_ACCL_SHIFT;
	offsets[1] = 0; // Y_ACCL_SHIFT;
	offsets[2] = 0; // Z_ACCL_SHIFT;
	offsets[3] = 0; // X_GYRO_SHIFT;
	offsets[4] = 0; // Y_GYRO_SHIFT;
	offsets[5] = 0; // Z_GYRO_SHIFT;
}


inline void debug2 (const char * name, int reg){
	// logger::debug("Register {:20s} : Value of Register {:2x}: {:5d}", name, reg, Read(reg));
}

inline void debug3(int val){
	// printf("[DEBUG] Changing value to %5d", val);
}

void mpu6050::print_debug(){
	debug2("Pwr Mng 1",REG_PWR_MNG_1);
	debug2("Cfg", REG_CFG);
	debug2("Accl Cfg", REG_ACCL_CFG);
	debug2("Gyro Cfg", REG_GYRO_CFG);

}

void mpu6050::wake_up(){
	Write(REG_PWR_MNG_1, Read(REG_PWR_MNG_1) & (~0b01000000));
	usleep(1000);
}

void mpu6050::sleep(){
	Write(REG_PWR_MNG_1, Read(REG_PWR_MNG_1) | 0b01000000);
	usleep(1000);
}

void mpu6050::set_accl_set(accl_range::accl_range set){
	switch(set){
	case accl_range::g_16:
    	accl_scale = 2048;
		break;
	case accl_range::g_8: 
    	accl_scale = 4096;
		break;
	case accl_range::g_4: 
    	accl_scale = 8192;
		break;
	case accl_range::g_2: 
    	accl_scale = 16384;
		break;
	}
	Write(REG_ACCL_CFG, Read(REG_ACCL_CFG) & (~0b00011000) | (set << 3));
	usleep(1000);
}

void mpu6050::set_gyro_set(gyro_range::gyro_range set){
	switch(set){
	case gyro_range::deg_250:
		gyro_scale = 131;
		break;
	case gyro_range::deg_500:
		gyro_scale = 65.5;
		break;
	case gyro_range::deg_1000:
		gyro_scale = 32.8;
		break;
	case gyro_range::deg_2000:
		gyro_scale = 16.4;
		break;
	}
	Write(REG_GYRO_CFG, Read(REG_GYRO_CFG) & (~0b00011000) | (set << 3));
	usleep(1000);


}

void mpu6050::set_clk(clk::clk set){
	Write(REG_PWR_MNG_1, Read(REG_PWR_MNG_1) & (~0b00000111) | set);
	usleep(1000);
}

void mpu6050::set_dlpf_bandwidth(dlpf::dlpf set){
	Write(REG_CFG, Read(REG_CFG) & (~0b00000111) | set);
	
	usleep(1000);
}

void mpu6050::set_fsync(fsync::fsync set){
	Write(REG_CFG, Read(REG_CFG) & (~0b00111000) | (set << 3));
	usleep(1000);
}

void mpu6050::set_pwr_set(int set){

}

int16_t handle_neg(int n){
	// if(n & 0x4000){
	// 	return -((~(n - 1))&0x7FFF);
	// }
	// return n&0x7FFF;
	return (int16_t) n;
}

int16_t combine(uint8_t h, uint8_t l) {
	return (((uint16_t) h) << 8) | l; 
}

void mpu6050::read_raw(int * data){
	uint8_t buf[14];
	mpu.read_burst(OUT_XACCL_H, buf, 14);
	data[0] = combine(buf[0],buf[1]);
	data[1] = combine(buf[2],buf[3]);
	data[2] = combine(buf[4],buf[5]);
	data[3] = combine(buf[8],buf[9]);
	data[4] = combine(buf[10],buf[11]);
	data[5] = combine(buf[12],buf[13]);
}

void mpu6050::read_wo_offsets(double * data){
	int rawdata[6];
	mpu6050::read_raw(rawdata);
	data[0] = ((double) rawdata[0]) / accl_scale;
	data[1] = ((double) rawdata[1]) / accl_scale;
	data[2] = ((double) rawdata[2]) / accl_scale;
	data[3] = ((double) rawdata[3]) / gyro_scale;
	data[4] = ((double) rawdata[4]) / gyro_scale;
	data[5] = ((double) rawdata[5]) / gyro_scale;
}


void mpu6050::read(double * data){
	int rawdata[6];
	mpu6050::read_raw(rawdata);
	data[0] = ((double) rawdata[0] - offsets[0]) / accl_scale;
	data[1] = ((double) rawdata[1] - offsets[1]) / accl_scale;
	data[2] = ((double) rawdata[2] - offsets[2]) / accl_scale;
	data[3] = ((double) rawdata[3] - offsets[3]) / gyro_scale;
	data[4] = ((double) rawdata[4] - offsets[4]) / gyro_scale;
	data[5] = ((double) rawdata[5] - offsets[5]) / gyro_scale;
}

int mpu6050::query_register(int reg){
	return Read(reg);
}

void mpu6050::set_register(int reg, int data){
	Write(reg,data);
}



// void mpu6050::calibrate(int n){
// 	int j = n;
//     int data[6];
// 	int s_data[6];
// 	for(int i = 0; i < 6; i++){s_data[i]=0;}
// 	printf(			"[Debug] Calibrating MPU6050\n");
// 	printf(			"[Debug] X Accl | Y Accl | Z Accl | X Gyro | Y Gyro | Z Gyro\n");
// 	while(j--){
// 		mpu6050::read_raw(data);
		// printf(	"[Debug] %6d | %6d | %6d | %6d | %6d | %6d\n",data[0],data[1],data[2],data[3],data[4],data[5]);
// 		for(int i = 0; i < 6; i++){
// 			s_data[i]+=data[i];

// 		}
//         usleep(500);
// 	}
// 	set_offsets(s_data[0] / n,s_data[1] / n,s_data[2] / n - accl_scale,s_data[3] / n,s_data[4] / n,s_data[5] / n);
// 	printf("\n\n[Output] Calibration Results: \n[Output] X Accl | Y Accl | Z Accl | X Gyro | Y Gyro | Z Gyro\n[Output] %6d | %6d | %6d | %6d | %6d | %6d\n[Output] The running program's offsets have been configured. To configure offsets when running other programs, insert the following line: \n[Output] mpu6050::set_offsets(%d, %d, %d, %d, %d, %d)\n\n", s_data[0] / n,s_data[1] / n,s_data[2] / n - 16834,s_data[3] / n,s_data[4] / n,s_data[5] / n, s_data[0] / n,s_data[1] / n,s_data[2] / n - 16834,s_data[3] / n,s_data[4] / n,s_data[5] / n);
// }


void mpu6050::calibrate(int n){
	int data[6];
	double error_sum[6];
	double kP[6] = {0.3, 0.3, 0.3, 0.3, 0.3, 0.3};
	double kI[6] = {90, 90, 90, 20, 20, 20};
	double expect[6] = {0, 0, accl_scale * 1.0, 0, 0, 0};
	for(int i = 0; i < 6; i++){
		offsets[i] = 0;
		error_sum[i] = 0;
	}

	for(int i = 0; i < n; i ++){
		for(int j = 0; j < 100; j ++){
			mpu6050::read_raw(data);
			// printf("[Debug] %6d | %6d | %6d | %6d | %6d | %6d\n",data[0],data[1],data[2],data[3],data[4],data[5]);
			// printf(	"[Debug] %6d | %6d | %6d | %6d | %6d | %6d\n",data[0],data[1],data[2],data[3],data[4],data[5]);
			
			double dt = 0.001;
			// logger::debug("{:6d} | {:6d} | {:6d} | {:6d} | {:6d} | {:6d}",data[0],data[1],data[2],data[3],data[4],data[5]);
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

	// logger::info("Offset: {:6d} | {:6d} | {:6d} | {:6d} | {:6d} | {:6d}",offsets[0], offsets[1], offsets[2], offsets[3], offsets[4], offsets[5]);
	// printf("\n\n[Output] Calibration Results: \n[Output] X Accl | Y Accl | Z Accl | X Gyro | Y Gyro | Z Gyro\n[Output] %6d | %6d | %6d | %6d | %6d | %6d\n[Output] The running program's offsets have been configured. To configure offsets when running other programs, insert the following line: \n[Output] mpu6050::set_offsets(%d, %d, %d, %d, %d, %d)\n\n", offsets[0], offsets[1], offsets[2], offsets[3], offsets[4], offsets[5], offsets[0], offsets[1], offsets[2], offsets[3], offsets[4], offsets[5]);

}

void mpu6050::set_offsets(int x_a, int y_a, int z_a, int x_g, int y_g, int z_g){
	offsets[0] = x_a;
	offsets[1] = y_a;
	offsets[2] = z_a;
	offsets[3] = x_g;
	offsets[4] = y_g;
	offsets[5] = z_g;
}