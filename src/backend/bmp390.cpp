#include <backend/bmp390.h>
#include <backend/bmp390_macros.h>

#include <backend/i2c.h>

// extern "C" {
// 	#include <linux/i2c-dev.h>
// 	#include <i2c/smbus.h>
// }

#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <cstdlib>
#include <cmath>
// #include <logger.h>

#define MOLAR_MASS_AIR 2.896e-2 // kg/mol
#define AVERAGE_SEA_LVL_PRESSURE 1.01325e5 // Pa
// #define PRESSURE_BENCHMARK AVERAGE_SEA_LVL_PRESSURE // Pa
#define PRESSURE_BENCHMARK 101533.678563 // Pa
#define STANDARD_TEMP 288.15 // K
#define UNV_GAS_CONST 8.3143 // (N*m) / (mol * K)
#define GRAVITATIONAL_ACCELERATION 9.807 // m/s^2

#define READ(reg) bmp.read_byte(reg) //i2c_smbus_read_byte_data(fd, reg) 
#define WRITE(reg, data) bmp.write_byte(reg, data) //i2c_smbus_write_byte_data(fd, reg, data) 

static i2c::device bmp; 
static double par_t1, par_t2, par_t3;
static double par_p1, par_p2, par_p3, par_p4, par_p5, par_p6, par_p7, par_p8, par_p9, par_p10, par_p11;


static double compensate_temp(uint32_t uncomp_temp){
    
    double partial_data1;
    double partial_data2;

    partial_data1 = (double)(uncomp_temp - par_t1);
    partial_data2 = (double)(partial_data1 * par_t2);

    /* Update the compensated temperature in calib structure since this is
     * needed for pressure calculation */
    return partial_data2 + (partial_data1 * partial_data1) * par_t3;
}

static double compensate_temp(){
    return compensate_temp(bmp390::get_raw_temp());
}

static double pow_bmp3(double base, uint8_t power) {
    double pow_output = 1;
    while (power != 0) {
        pow_output = (double) base * pow_output;
        power--;
    }
    return pow_output;
}

static double compensate_pressure(uint32_t uncomp_pressure, double temp) {
    double t_lin = temp;
    /* Variable to store the compensated pressure */
    double comp_press;

    /* Temporary variables used for compensation */
    double partial_data1;
    double partial_data2;
    double partial_data3;
    double partial_data4;
    double partial_out1;
    double partial_out2;

    partial_data1 = par_p6 * t_lin;
    partial_data2 = par_p7 * pow_bmp3(t_lin, 2);
    partial_data3 = par_p8 * pow_bmp3(t_lin, 3);
    partial_out1 = par_p5 + partial_data1 + partial_data2 + partial_data3;
    partial_data1 = par_p2 * t_lin;
    partial_data2 = par_p3 * pow_bmp3(t_lin, 2);
    partial_data3 = par_p4 * pow_bmp3(t_lin, 3);
    partial_out2 = uncomp_pressure *
                   (par_p1 + partial_data1 + partial_data2 + partial_data3);
    partial_data1 = pow_bmp3((double)uncomp_pressure, 2);
    partial_data2 = par_p9 + par_p10 * t_lin;
    partial_data3 = partial_data1 * partial_data2;
    partial_data4 = partial_data3 + pow_bmp3((double)uncomp_pressure, 3) * par_p11;
    comp_press = partial_out1 + partial_out2 + partial_data4;


    return comp_press - 1024;
}

static double compensate_pressure() {
    return compensate_pressure(bmp390::get_raw_press(), compensate_temp());
}

static double compensate_pressure(double temp) {
    return compensate_pressure(bmp390::get_raw_press(), temp);
}

static int16_t combine(int byte1, int byte2) {
  // This code assumes that byte1 is in range, but allows for the possibility
  // that the values were originally in a signed char and so is now negative.
  return (int16_t) (((uint16_t) byte1 << 8) | byte2);
}

static inline uint32_t combine(uint8_t h, uint8_t l, uint8_t xl) { 
    return  (((uint32_t) h) << 16) | (((uint32_t) l) << 8) |(((uint32_t) xl));
}

void bmp390::init(){
    bmp = i2c::device(BMP390_ADDR);
    bmp390::acquire_calib_vars();
}


void bmp390::stop(){
    bmp.close();
}

int bmp390::query_register(int reg){
    return READ(reg);
}

void bmp390::soft_reset(){
    // std::cout << "FUCK SHIT CUNT DICK\n";
    WRITE(BMP390_REG_CMD, BMP390_SOFT_RESET);
}


void bmp390::set_oversample(bmp390::oversampling pressure, bmp390::oversampling temperature){
    WRITE(BMP390_REG_OSR, pressure | (temperature << 3));
}
void bmp390::set_pressure_oversample(bmp390::oversampling pressure){
    WRITE(BMP390_REG_OSR, (READ(BMP390_REG_OSR) & (~0b00000111)) | pressure);
}
void bmp390::set_temperature_oversample(bmp390::oversampling temperature){
    WRITE(BMP390_REG_OSR, (READ(BMP390_REG_OSR) & (~0b00111000)) | (temperature << 3));
}
void bmp390::set_output_data_rate(bmp390::output_data_rate rate){
    WRITE(BMP390_REG_ODR, rate);
}
void bmp390::set_pwr_mode(bmp390::pwr mode){
    int k = (READ(BMP390_REG_PWR_CTRL) & (~0b00110000)) | (mode << 4);
    // logger::info("bruh {:x}", k);
    WRITE(BMP390_REG_PWR_CTRL, k);
}
void bmp390::set_enable_pressure(bool enable){
    WRITE(BMP390_REG_PWR_CTRL, (READ(BMP390_REG_PWR_CTRL) & (~0b00000001)) | (enable));
}
void bmp390::set_enable_temperature(bool enable){
    WRITE(BMP390_REG_PWR_CTRL, (READ(BMP390_REG_PWR_CTRL) & (~0b00000010)) | (enable << 1));
}
void bmp390::set_enable(bool pressure, bool temperature){
    WRITE(BMP390_REG_PWR_CTRL, (READ(BMP390_REG_PWR_CTRL) & (~0b00000011)) | (temperature << 1) | pressure);
}
void bmp390::set_iir_filter(bmp390::iir_filter filter) {
    WRITE(BMP390_REG_CONFIG, filter << 1);
}
void bmp390::set_pwr_ctrl(int val){
    WRITE(BMP390_REG_PWR_CTRL, val);
}

void bmp390::set_enable_fifo(bool enable) {
    WRITE(BMP390_FIFO_CONFIG_1, (READ(BMP390_FIFO_CONFIG_1) & (~0b00000001)) | (enable));
}
void bmp390::set_enable_fifo(bool pressure, bool temperature) {
    WRITE(BMP390_FIFO_CONFIG_1, (READ(BMP390_FIFO_CONFIG_1) & (~0b00011001)) | (pressure || temperature) | (pressure << 3) | (temperature << 4));
}
void bmp390::set_enable_fifo_time(bool enable) {
    WRITE(BMP390_FIFO_CONFIG_1, (READ(BMP390_FIFO_CONFIG_1) & (~0b00000101)) | (enable) | (enable << 2));
}
void bmp390::set_fifo_stop_on_full(bool stop_on_full) {
    WRITE(BMP390_FIFO_CONFIG_1, (READ(BMP390_FIFO_CONFIG_1) & (~0b00000010)) | (stop_on_full << 1));
}

void bmp390::read_fifo(double * data) {
    uint8_t frames_w_len[514];
    bmp.read_burst(BMP390_FIFO_LENGTH_0, frames_w_len, 514);
    int len = ((uint16_t) frames_w_len[0]) | (((uint16_t) frames_w_len[1]) << 8);
    // logger::info("FIFO Length: {:#04x} {:#04x} {:d}", frames_w_len[1], frames_w_len[0], len);

    // Sensor Frame:             0b10----00
    // Ctrl Frame: Config Error: 0b01000100
    // Ctrl Frame: Config Chg:   0b01001000

    // fmt binary: {:#010b}

    int i = 2;
    len += 2;

    data[0] = 0;
    data[1] = 0;
    data[2] = 0;

    int n_readings = 0;

    while(i < len) {
        uint8_t frame_type = (frames_w_len[i] & 0b11000000);
        uint8_t frame_param = (frames_w_len[i] & 0b00111100);
        // logger::info("FIFO header: {:#010b}", frames_w_len[i]);
        i++;

        if(frame_type == 0b10000000) { // sensor frame
            if(frame_param == 0b00010100) { 
                uint32_t raw_temp = combine(frames_w_len[i+2], frames_w_len[i+1], frames_w_len[i]);
                // logger::info("Raw temp: {:#010b}{:08b}{:08b} {:d}", frames_w_len[i+2], frames_w_len[i+1], frames_w_len[i], raw_temp);
                i+=3;
                uint32_t raw_press = combine(frames_w_len[i+2], frames_w_len[i+1], frames_w_len[i]);
                // logger::info("Raw press: {:#010b}{:08b}{:08b} {:d}", frames_w_len[i+2], frames_w_len[i+1], frames_w_len[i], raw_press);
                i+=3;
                
                double temp = compensate_temp(raw_temp);
                data[0] += temp;
                // data[0] += compensate_temp(raw_temp);
                data[1] += compensate_pressure(raw_press, temp);
                
                n_readings ++;
                // #ifdef FAST_FIFO
                // break;
                // #endif
            }else if(frame_param == 0b00100000) {
                uint32_t sensor_time = combine(frames_w_len[i+2], frames_w_len[i+1], frames_w_len[i]);
                // logger::info("Sensor time: {:#010b}{:08b}{:08b} {:d}", frames_w_len[i+2], frames_w_len[i+1], frames_w_len[i], sensor_time);
                i+=3;
            }
            // logger::info("FIFO data");   
        }else { // control frame
            i++;
        }
    }
    data[0] /= n_readings;
    data[1] /= n_readings;
    data[2] = bmp390::get_height(data[0], data[1]);
}

void bmp390::read_fifo_wo_height(double * data) {
    uint8_t frames_w_len[514];
    bmp.read_burst(BMP390_FIFO_LENGTH_0, frames_w_len, 514);
    int len = ((uint16_t) frames_w_len[0]) | (((uint16_t) frames_w_len[1]) << 8);
    // logger::info("FIFO Length: {:#04x} {:#04x} {:d}", frames_w_len[1], frames_w_len[0], len);

    // Sensor Frame:             0b10----00
    // Ctrl Frame: Config Error: 0b01000100
    // Ctrl Frame: Config Chg:   0b01001000

    // fmt binary: {:#010b}

    int i = 2;
    len += 2;

    data[0] = 0;
    data[1] = 0;
    data[2] = 0;

    int n_readings = 0;

    while(i < len) {
        uint8_t frame_type = (frames_w_len[i] & 0b11000000);
        uint8_t frame_param = (frames_w_len[i] & 0b00111100);
        // logger::info("FIFO header: {:#010b}", frames_w_len[i]);
        i++;

        if(frame_type == 0b10000000) { // sensor frame
            if(frame_param == 0b00010100) { 
                uint32_t raw_temp = combine(frames_w_len[i+2], frames_w_len[i+1], frames_w_len[i]);
                // logger::info("Raw temp: {:#010b}{:08b}{:08b} {:d}", frames_w_len[i+2], frames_w_len[i+1], frames_w_len[i], raw_temp);
                i+=3;
                uint32_t raw_press = combine(frames_w_len[i+2], frames_w_len[i+1], frames_w_len[i]);
                // logger::info("Raw press: {:#010b}{:08b}{:08b} {:d}", frames_w_len[i+2], frames_w_len[i+1], frames_w_len[i], raw_press);
                i+=3;
                
                double temp = compensate_temp(raw_temp);
                data[0] += temp;
                // data[0] += compensate_temp(raw_temp);
                data[1] += compensate_pressure(raw_press, temp);
                
                n_readings ++;
                #ifdef FAST_FIFO
                break;
                #endif
            }else if(frame_param == 0b00100000) {
                uint32_t sensor_time = combine(frames_w_len[i+2], frames_w_len[i+1], frames_w_len[i]);
                // logger::info("Sensor time: {:#010b}{:08b}{:08b} {:d}", frames_w_len[i+2], frames_w_len[i+1], frames_w_len[i], sensor_time);
                i+=3;
            }
            // logger::info("FIFO data");   
        }else { // control frame
            i++;
        }
    }
    data[0] /= n_readings;
    data[1] /= n_readings;
}

void bmp390::flush_fifo() {
    WRITE(BMP390_REG_CMD, BMP390_FIFO_FLUSH);
}

void bmp390::acquire_calib_vars(){
    #define BMP3_CONCAT_BYTES(msb, lsb)             (((uint16_t)msb << 8) | (uint16_t)lsb)
    uint16_t reg_par_t1;
    uint16_t reg_par_t2;
    int8_t reg_par_t3;
    int16_t reg_par_p1;
    int16_t reg_par_p2;
    int8_t reg_par_p3;
    int8_t reg_par_p4;
    uint16_t reg_par_p5;
    uint16_t reg_par_p6;
    int8_t reg_par_p7;
    int8_t reg_par_p8;
    int16_t reg_par_p9;
    int8_t reg_par_p10;
    int8_t reg_par_p11;

    uint8_t reg_data[20];

    bmp.read_burst(NVM_PAR_T1_L, reg_data, 20);
    
    /* Temporary variable */
    double temp_var;

    /* 1 / 2^8 */
    temp_var = 0.00390625f;
    reg_par_t1 = BMP3_CONCAT_BYTES(reg_data[1], reg_data[0]);
    par_t1 = ((double)reg_par_t1 / temp_var);
    reg_par_t2 = BMP3_CONCAT_BYTES(reg_data[3], reg_data[2]);
    temp_var = 1073741824.0f;
    par_t2 = ((double)reg_par_t2 / temp_var);
    reg_par_t3 = (int8_t)reg_data[4];
    temp_var = 281474976710656.0f;
    par_t3 = ((double)reg_par_t3 / temp_var);
    reg_par_p1 = (int16_t)BMP3_CONCAT_BYTES(reg_data[6], reg_data[5]);
    temp_var = 1048576.0f;
    par_p1 = ((double)(reg_par_p1 - (16384)) / temp_var);
    reg_par_p2 = (int16_t)BMP3_CONCAT_BYTES(reg_data[8], reg_data[7]);
    temp_var = 536870912.0f;
    par_p2 = ((double)(reg_par_p2 - (16384)) / temp_var);
    reg_par_p3 = (int8_t)reg_data[9];
    temp_var = 4294967296.0f;
    par_p3 = ((double)reg_par_p3 / temp_var);
    reg_par_p4 = (int8_t)reg_data[10];
    temp_var = 137438953472.0f;
    par_p4 = ((double)reg_par_p4 / temp_var);
    reg_par_p5 = BMP3_CONCAT_BYTES(reg_data[12], reg_data[11]);

    /* 1 / 2^3 */
    temp_var = 0.125f;
    par_p5 = ((double)reg_par_p5 / temp_var);
    reg_par_p6 = BMP3_CONCAT_BYTES(reg_data[14], reg_data[13]);
    temp_var = 64.0f;
    par_p6 = ((double)reg_par_p6 / temp_var);
    reg_par_p7 = (int8_t)reg_data[15];
    temp_var = 256.0f;
    par_p7 = ((double)reg_par_p7 / temp_var);
    reg_par_p8 = (int8_t)reg_data[16];
    temp_var = 32768.0f;
    par_p8 = ((double)reg_par_p8 / temp_var);
    reg_par_p9 = (int16_t)BMP3_CONCAT_BYTES(reg_data[18], reg_data[17]);
    temp_var = 281474976710656.0f;
    par_p9 = ((double)reg_par_p9 / temp_var);
    reg_par_p10 = (int8_t)reg_data[19];
    temp_var = 281474976710656.0f;
    par_p10 = ((double)reg_par_p10 / temp_var);
    reg_par_p11 = (int8_t)reg_data[20];
    temp_var = 36893488147419103232.0f;
    par_p11 = ((double)reg_par_p11 / temp_var);
}

int bmp390::get_raw_press(){
    uint8_t data[3];
    bmp.read_burst(BMP390_REG_PRESS_7_0, data, 3);
    return combine(data[2], data[1], data[0]);
}

double bmp390::get_press(){
    return compensate_pressure();
}

double bmp390::get_press(double temp){
    return compensate_pressure(temp);
}

// #define p0 101325
static double p0 = 101325;

void bmp390::set_pressure_benchmark(double _p0){
    p0 = _p0;
}

double height(double temp_c, double pressure_k){
    double temp_k = temp_c + 273.15;

    return (-1 + pow((p0/pressure_k),(1/5.255))) * temp_k / 0.0065;
    // return - UNV_GAS_CONST * temp_k * log(pressure_k / PRESSURE_BENCHMARK) / (MOLAR_MASS_AIR * GRAVITATIONAL_ACCELERATION);
}

double bmp390::get_height(double temp, double press){
    return height(temp, press);
}

double bmp390::get_height(){
    double temp_c = get_temp();
    double pressure_P = get_press(temp_c);
    return height(temp_c, pressure_P);
}

int bmp390::get_raw_temp(){
    uint8_t data[3];
    bmp.read_burst(BMP390_REG_TEMP_7_0, data, 3);
    return combine(data[2], data[1], data[0]);
}

void bmp390::get_data(double * data){
    data[0] = bmp390::get_temp();
    data[1] = bmp390::get_press(data[0]);
    data[2] = height(data[0], data[1]);
}

double bmp390::get_temp(){
    return compensate_temp();
}



