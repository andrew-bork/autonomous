#include <backend/pca9685.h>
#include <backend/pca9685_macros.h>

#include <backend/i2c.h>

#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#define read(r) device.read_byte((uint8_t) r)
#define write(r,v) device.write_byte((uint8_t) r, (uint8_t) v)


static int pwm_regs[16][4] = {
    {0x06, 0x07, 0x08, 0x09},
    {0x0A, 0x0B, 0x0C, 0x0D},
    {0x0E, 0x0F, 0x10, 0x11},
    {0x02, 0x03, 0x14, 0x15},
    {0x16, 0x17, 0x18, 0x19},
    {0x1A, 0x1B, 0x1C, 0x1D},
    {0x1E, 0x1F, 0x20, 0x21},
    {0x22, 0x23, 0x24, 0x25},
    {0x26, 0x27, 0x28, 0x29},
    {0x2A, 0x2B, 0x2C, 0x2D},
    {0x2E, 0x2F, 0x30, 0x31},
    {0x32, 0x33, 0x34, 0x35},
    {0x36, 0x37, 0x38, 0x39},
    {0x3A, 0x3B, 0x3C, 0x3D},
    {0x3E, 0x3F, 0x40, 0x41},
    {0x42, 0x43, 0x44, 0x45},
};

void pca9685::get_frequency_from_device(){
    frequency = 25000000 / ((read(PRESCALE) + 1) * 4096);
}

int pca9685::get_frequency() {
    return frequency;
}

pca9685::pca9685() : device(PCA9685_ADDRESS){
    get_frequency_from_device();
    period = 1000000 / frequency;
}

pca9685::~pca9685(){
    device.close();
}

int pca9685::query_reg(std::uint8_t register_address){
    return device.read_byte((uint8_t) register_address);
}

void pca9685::write_reg(std::uint8_t register_address, std::uint8_t value){
    device.write_byte(register_address, value);
}

static inline int pcaround(double d){
    if(d < 0){
        return (((-d) - ((int)-d)) < 0.5 ? ((int) d) : ((int) d - 1));
    }
    return (d - ((int) d) < 0.5 ? (int) d : (int) d + 1);
}

void pca9685::set_frequency(int _frequency){
    // PRESCALE_VAL -> round(25000000 / (4096 * update_rate)) - 1
    frequency = _frequency;
    period = 1000000 / frequency;
    int reg_val = pcaround(25000000.0 / (4096 * frequency)) - 1;

    write(PRESCALE, reg_val & 0xFF);
    usleep(5000);
}

void pca9685::set_pwm_ms(std::uint8_t pwm, std::uint16_t micro_s){
    int cycle = 4096 * micro_s / period;
    write(pwm_regs[pwm][0], 0);
    write(pwm_regs[pwm][1], 0);
    write(pwm_regs[pwm][2], cycle & 0xFF);
    write(pwm_regs[pwm][3], (cycle >> 8) & 0xF);
}
void pca9685::set_pwm_percent(std::uint8_t pwm, double percent){
    int cycle = (int) (4096 * percent);
    write(pwm_regs[pwm][0], 0);
    write(pwm_regs[pwm][1], 0);
    write(pwm_regs[pwm][2], cycle & 0xFF);
    write(pwm_regs[pwm][3], (cycle >> 8) & 0xF);
}

void pca9685::set_pwm_on(std::uint8_t pwm, std::uint16_t on){
    write(pwm_regs[pwm][0], on & 0xFF);
    write(pwm_regs[pwm][1], (on >> 8) & 0xF);
}

void pca9685::set_pwm_off(std::uint8_t pwm, std::uint16_t off){
    
}

bool pca9685::is_awake() {
    return (read(MODE_1) & 0b00010000) == 0;
}

void pca9685::wake_up(){
    std::uint8_t old = read(MODE_1);
    write(MODE_1, old & ~(0b00010000));
    // usleep(5000);
}

void pca9685::sleep(){
    std::uint8_t old = read(MODE_1) & 0xFF;
    write(MODE_1, old | 0b00010000);
    // usleep(5000);
}

void pca9685::restart(){

}