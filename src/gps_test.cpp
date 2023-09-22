#include <backend/gps.h>

int main() {
    gps device;
    device.init();

    while(1) {
        if(device.new_data) {
            printf("%f, %f\n", device.lat, device.lng);
            device.new_data = false;
        }
        usleep(100000);
    }
}