#include <backend/i2c.h>
#include <backend/qmc5883.h>
#include <cstdint>



#define DATA_OUTPUT_X_LSB 0x00
#define DATA_OUTPUT_X_MSB 0x01
#define DATA_OUTPUT_Y_LSB 0x02
#define DATA_OUTPUT_Y_MSB 0x03
#define DATA_OUTPUT_Z_LSB 0x04
#define DATA_OUTPUT_Z_MSB 0x05

#define STATUS_REG 0x06

#define CONTROL_REGISTER_1 0x09

#define I2C_ADDR 0x0d   


static i2c::device qmc; 


void qmc5883::init(){
    qmc = i2c::device(I2C_ADDR);
    // bmp390::acquire_calib_vars();
}


void qmc5883::set_calibration_values(double * values) {

}


static int16_t combine(int byte1, int byte2) {
  // This code assumes that byte1 is in range, but allows for the possibility
  // that the values were originally in a signed char and so is now negative.
  return (int16_t) (((int16_t) byte1 << 8) | byte2);
}

void qmc5883::get_data(double * data){
    uint8_t raw[6];
    qmc.read_burst(DATA_OUTPUT_X_LSB, raw, 6);
}
void qmc5883::get_raw_data(int16_t * raw_data){
    uint8_t raw[6];
    qmc.read_burst(DATA_OUTPUT_X_LSB, raw, 6);
    raw_data[0] = combine(raw[1], raw[0]);
    raw_data[1] = combine(raw[3], raw[2]);
    raw_data[2] = combine(raw[5], raw[4]);
}

void qmc5883::set_control_register(mode _mode, odr _odr, range _range, osr _osr) {
    qmc.write_byte(CONTROL_REGISTER_1, _mode | (_odr << 2) | (_range << 4) | (_osr << 6));
}

bool qmc5883::is_data_ready(){
    uint8_t a = qmc.read_byte(STATUS_REG);
    return (a & 0b00000100) == 0b00000100;
}