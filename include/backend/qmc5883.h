#ifndef QMC5883_H
#define QMC5883_H
#include <cstdint>

namespace qmc5883 {
    enum mode {
        continuous=0b01,
        standby=0b00,
    };

    enum odr {
        hz10=0b00,
        hz50=0b01,
        hz100=0b10,
        hz200=0b11
    };

    enum range {
        g2=0b00,
        g8=0b01,
    };

    enum osr {
        k512=0b00,
        k256=0b01,
        k128=0b10,
        k64=0b11
    };

    void set_calibration_values(double * values);

    void get_data(double * data);
    void get_raw_data(int16_t * raw_data);

    bool is_data_ready();

    void set_control_register(mode _mode, odr _odr, range _range, osr _osr);

    void init();
}

#endif