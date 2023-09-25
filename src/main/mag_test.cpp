#include <backend/qmc5883.h>
#include <cstdio>
#include <unistd.h>
#include <fstream>
#include <cmath>
#include <iostream>
#include <math/filter.h>



int main() {

    filter::filter filters[3];
    for(int i = 0; i < 3; i ++){
        filters[i] = filter::low_pass(100, 2);
    }

    std::ofstream out("mag-data.csv");

    qmc5883::init();
    qmc5883::set_control_register(qmc5883::continuous, qmc5883::hz200, qmc5883::g2, qmc5883::k64);

    double data[3];
    while(1) {;

        qmc5883::get_data(data);

        for(int i = 0; i < 3; i ++){
            data[i] = filters[i][data[i]];
        }
        std::cout << data[0] << "," << data[1] << "," << data[2] << std::endl;

        usleep(1000000/100);
    }
}

