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

static double rng = 2.0f;
// static double calib_values = 

void qmc5883::init(){
    qmc = i2c::device(I2C_ADDR);
    // bmp390::acquire_calib_vars();
}
/**
 * Center at      0.2534,    0.1855,   -0.0026
 * 478.94602207 143.82156333  11.8942917 ]
 [143.82156333 203.5568407   28.90751369]
 [ 11.8942917   28.90751369  14.9442476 
 * 
 */

static double off_x = 0.2534, off_y = 0.1855, off_z = -0.0026;

static double ax = 478.94602207, ay = 143.82156333, az = 11.8942917;
static double bx = 143.82156333, by = 203.5568407, bz = 28.90751369;
static double cx = 11.8942917, cy = 28.90751369, cz = 14.9442476;

void qmc5883::set_calibration_values(double * values) {

}


static int16_t combine(uint8_t byte1, uint8_t byte2) {
  // This code assumes that byte1 is in range, but allows for the possibility
  // that the values were originally in a signed char and so is now negative.
  return (int16_t) (((int16_t) byte1 << 8) | byte2);
}

void qmc5883::get_data(double * data){
    int16_t raw_data[3];
    get_raw_data(raw_data);

    double scaled[3];

    // scaled[0] = raw_data[0] * rng - off_x * rng / (2 / (65536.0));
    // scaled[1] = raw_data[1] * rng - off_y * rng / (2 / (65536.0));
    // scaled[2] = raw_data[2] * rng - off_z * rng / (2 / (65536.0));

    // data[0] = ax * scaled[0] + bx * scaled[1] + cx * scaled[2];
    // data[1] = ay * scaled[0] + by * scaled[1] + cy * scaled[2];
    // data[2] = az * scaled[0] + bz * scaled[1] + cz * scaled[2];

    data[0] = raw_data[0] * rng;
    data[1] = raw_data[1] * rng;
    data[2] = raw_data[2] * rng;
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
    switch(_range) {
    case g2:
        rng = 2 / (65536.0);
        break;
    case g8:
        rng = 8 / (65536.0);
        break;
    }
}

bool qmc5883::is_data_ready(){
    uint8_t a = qmc.read_byte(STATUS_REG);
    return (a & 0b00000100) == 0b00000100;
}