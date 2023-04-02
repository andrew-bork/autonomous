#include <backend/bmp390.h>
#include <unistd.h>
#include <cstdio>
#include <util/timer.h>
#include <math/filter.h>

#define M_TO_FT 3.28084

double data[3];
// void tick() {
//     bmp390::get_data(data);
//     printf("%4f %4f %4f\n", data[0], data[1], data[2]);
// }

int main(int argc, char** argv) {
    double samp_rate = 20.0, cutoff = 1.0;
    if(argc > 1) {
        samp_rate = atof(argv[1]);
    }
    if(argc > 2) {
        cutoff = atof(argv[2]);
    }
    
    bmp390::init();
    
    // timer t;
    // t.start(tick, 1000);
    // t.start();
    bmp390::init();
    bmp390::soft_reset();
    bmp390::set_oversample(bmp390::oversampling::HIGH, bmp390::ULTRA_LOW_POWER);
    bmp390::set_iir_filter(bmp390::COEFF_15);
    bmp390::set_output_data_rate(bmp390::hz50);
    bmp390::set_enable(true, true);

    // bmp390::set_enable_fifo(true, true);
    bmp390::set_fifo_stop_on_full(false);

    bmp390::set_pwr_mode(bmp390::NORMAL);
    double then, vs;
    double fthen, fvs;
    double filtered;

    filter::filter f;
    f = filter::low_pass(samp_rate, cutoff);

    double init_alt = 0.0;
    int sleep = 1000000 / samp_rate;
    int i = 50;
    while(i--) {
        bmp390::get_data(data);
        filtered = f[data[2]];
        usleep(sleep);
    }
    init_alt = data[2];






    while(1){
        bmp390::get_data(data);
        // bmp390::read_fifo(data);
        
        vs = (data[2] - then) / 1.0;
        then = data[2];

        filtered = f[data[2]];
        fvs = (filtered - fthen) / 1.0;
        fthen = filtered;

        printf("| %3.1f °C | %2d kP | %5.2f ft | %5.2f ft/s | %5.2f ft | %5.2f ft/s |\n", data[0], (int) (data[1] / 1000), (data[2] - init_alt) * M_TO_FT, vs * M_TO_FT, (filtered - init_alt) * M_TO_FT, fvs * M_TO_FT);
        usleep(sleep);
    }
}