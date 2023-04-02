#ifndef I2C_H_GUARD
#define I2C_H_GUARD
#include <cstdint>

namespace i2c{

    // int get_device(int addr);
    // int close_device(int fd);

    // uint8_t read_byte(int fd, uint8_t reg);
    // void read_burst(int fd, uint8_t reg, uint8_t * buf, int len);

    // void write_byte(int fd, uint8_t reg, uint8_t val);
    // void write_burst(int fd, uint8_t reg, uint8_t * buf, int len);
    struct device {
        int fd;
        int addr;

        device();
        device(int addr);

        void close();

        uint8_t read_byte(uint8_t reg);
        void read_burst(uint8_t reg, uint8_t * buf, int len);

        void write_byte(uint8_t reg, uint8_t val);
        void write_burst(uint8_t reg, uint8_t * buf, int len);
    };
}

#endif