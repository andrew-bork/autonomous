#pragma once
#include <backend/i2c.h>
struct bmp390 {

    enum pwr {
        SLEEP = 0b00, NORMAL = 0b11, FORCED = 0b10
    };
    
    enum oversampling {
        ULTRA_LOW_POWER = 0b000,
        LOW_POWER = 0b001,
        STANDARD = 0b010,
        HIGH = 0b011,
        ULTRA_HIGH = 0b100,
        HIGHEST = 0b101
    };

    enum iir_filter {
        COEFF_0 = 0b000,
        COEFF_1 = 0b001,
        COEFF_3 = 0b010,
        COEFF_7 = 0b011,
        COEFF_15 = 0b100,
        COEFF_31 = 0b101,
        COEFF_63 = 0b110,
        COEFF_127 = 0b111
    };

    enum output_data_rate {
        hz200 = 0x00, // 200 hz
        hz100 = 0x01,
        hz50 = 0x02,
        hz25 = 0x03,
        hz25_2 = 0x04,
        hz25_4 = 0x05,
        hz25_8 = 0x06,
        hz25_16 = 0x07,
        hz25_32 = 0x08,
        hz25_64 = 0x09,
        hz25_128 = 0x0A,
        hz25_256 = 0x0B,
        hz25_512 = 0x0C,
        hz25_1024 = 0x0D,
        hz25_2048 = 0x0E,
        hz25_4096 = 0x0F,
        hz25_8192 = 0x10,
        hz25_16384 = 0x11,
    };

    bmp390();
    ~bmp390();

    void acquire_calib_vars();

    int get_raw_temp();
    int get_raw_press();

    struct reading {
        double temperature;
        double pressure;
    };
    
    reading get_data();
    void get_data(double * data);

    void set_iir_filter(iir_filter coeff);
    void set_oversample(oversampling pressure, oversampling temperature);
    void set_pressure_oversample(oversampling pressure);
    void set_temperature_oversample(oversampling temperature);
    void set_output_data_rate(output_data_rate rate);
    void set_pwr_mode(pwr mode);
    void set_enable_pressure(bool enabled);
    void set_enable_temperature(bool enabled);
    void set_enable(bool pressure, bool temperature);
    void set_pwr_ctrl(int val);

    void set_enable_fifo(bool enable);
    void set_enable_fifo(bool pressure, bool temperature);
    void set_enable_fifo_time(bool enable);
    void set_fifo_stop_on_full(bool stop_on_full);
    void flush_fifo();

    void read_fifo(double * data);
    void read_fifo_wo_height(double * data);

    void set_pressure_benchmark(double p0);

    int chk_error();
    int print_error();

    int query_register(int reg);

    void soft_reset();

    private:

        double compensate_temperature(std::uint32_t raw_temperature);
        double compensate_pressure(double temperature, std::uint32_t raw_pressure);
        struct {
            double par_t1, par_t2, par_t3;
            double par_p1, par_p2, par_p3, par_p4, par_p5, par_p6, par_p7, par_p8, par_p9, par_p10, par_p11;
        } coeffs;

        i2c::device device;
};
