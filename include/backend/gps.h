#include <util/str_utils.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <vector>

#include <thread>

#define GPS_INTERNAL_BUFFER 1024
struct nmea_result {
    bool useful;
    bool fix;
    double lng, lat;
    short n_sat;

    bool north, east;
};


struct gps {
    int fd;
    bool new_data = false;

    bool fix = false;

    nmea_result last_useful_nmea_result;

    double lat, lng;

    std::thread thread;
    void init();

    static void gps_thread(gps* gps_dev);
};