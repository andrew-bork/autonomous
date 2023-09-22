#include <sys/socket.h>
#include <sys/un.h>
#include <cstring>
#include <cstdio>
#include <thread>
#include <util/timer.h>
#include <unistd.h>
#include <math/math.h>

#include <signal.h>
#include <termios.h>


double data[32];

double * mpu6050_data = data; // 0 - 5
double * filtered_mpu6050_data = data + 6; // 6 - 11
double * euler_angles = data + 12; // 12 - 14

double * bmp390_data = data + 15; // 15 - 17

double * qmc5883_data = data + 18; // 18 - 20
double * gps_data = data + 21; // 21 - 23

double * debug = data + 24; // 24

void draw_power_bar(const char * title, int width, double pwr) {
    int n = pwr * width;
    int inv_n = width - n;

    printf("\x1b[2K%10s: ", title);
    for(int i = 0; i < width; i ++) {
        if(i < n)
            printf("█");
        else
            putchar(' ');
    }
    printf("   %3.1f%%\n", pwr * 100);
}

namespace logger {
    char * msgs[20];
    bool updated = false;
    void init() {
        for(size_t i = 0; i < 20; i ++) msgs[i] = NULL;
    }
    void log(const char * msg) {
        free(msgs[0]);
        for(size_t i = 1; i < 20; i++) {
            msgs[i-1] = msgs[i];
        }
        size_t n = strlen(msg);
        msgs[19] = (char *) malloc(n+1);
        strncpy(msgs[19], msg, n+1);
        updated = true;
    }
    void draw() {
        if(!updated) return;
        updated = false;
        for(size_t i = 0; i < 20; i ++) {
            // fputs("\x1b[70;10H", stdout);
            if(msgs[i] == NULL) break;
            printf("\x1b[%d;10H%s", 70 + i, msgs[i]);
        }
        // fputs(msgs[])
    }
};

struct cli_table {
    char buffer[2048];

    short x = 0, y = 1;
    size_t i = 0;

    void init(size_t n_r, size_t n_c) {
        for(size_t r = 0; r < n_r; r ++) {
            char fmt_buf[16];
            snprintf(fmt_buf, 16, "\x1b[%d;%dH", y + r * 2 + 0, x);
            _cpy_multi_char(fmt_buf);

            if(r == 0) _gen_row(n_c, "╔", "═", "╤", "╗");            // ╔═════════╤═════════╤═════════╤═════════╗
            else _gen_row(n_c, "╟", "─", "┼", "╢");                  // ╟─────────┼─────────┼─────────┼─────────╢
            
            
            snprintf(fmt_buf, 16, "\x1b[%d;%dH", y + r * 2 + 1, x);
            _cpy_multi_char(fmt_buf);

            _gen_row(n_c, "║", " ", "│", "║");                       // ║         │         │         │         ║
            
            if(r == (n_r - 1)) {
                snprintf(fmt_buf, 16, "\x1b[%d;%dH", y + r * 2 + 2, x);
                _cpy_multi_char(fmt_buf);
                _gen_row(n_c, "╚", "═", "╧", "╝");    // ╚═════════╧═════════╧═════════╧═════════╝
            
            }
        }

        // buffer[i++] = '\0';
    }

    void _cpy_multi_char(const char* str, size_t n){
        for(size_t j = 0; str[j] != '\0' && j != n; j ++) {
            buffer[i++] = str[j];
        }
    }

    void _cpy_multi_char(const char* str){
        for(size_t j = 0; str[j] != '\0'; j ++) {
            buffer[i++] = str[j];
        }
    }

    void _gen_row(size_t& n_c, const char* left, const char* mid, const char* mid_sep, const char* right) {
        // buffer[i++] = left;
        _cpy_multi_char(left);
        for(size_t c = 0; c < n_c; c ++) {
            for(short k = 0; k < 9; k ++) {
                // buffer[i++] = mid;
                _cpy_multi_char(mid);
            }
            if(c == (n_c - 1)) {
                // buffer[i++] = right;
                _cpy_multi_char(right);
            }else {
                // buffer[i++] = mid_sep;
                _cpy_multi_char(mid_sep);
            }
        }
    }

    void set_color(int color) {
        
    }

    void set_fg_color(int r, int g, int b) {
        char fmt_buf[32];
        snprintf(fmt_buf, 32, "\x1b[38;2;%d;%d;%dm", r, g, b);
        _cpy_multi_char(fmt_buf);
    }

    void set_bg_color(int r, int g, int b) {
        char fmt_buf[32];
        snprintf(fmt_buf, 32, "\x1b[48;2;%d;%d;%dm", r, g, b);
        _cpy_multi_char(fmt_buf);
    }

    void set_bold() {
        _cpy_multi_char("\x1b[1m");
    }

    void set_faint() {
        _cpy_multi_char("\x1b[2m");
    }

    void set_italic() {
        _cpy_multi_char("\x1b[3m");
    }

    void set_underline() {
        _cpy_multi_char("\x1b[4m");

    }

    void set_blinking() {
        _cpy_multi_char("\x1b[5m");
    }

    void set_inverse() {
        _cpy_multi_char("\x1b[6m");
    }

    void set_invisible() {
        _cpy_multi_char("\x1b[7m");
    }

    void set_strikethrough() {
        _cpy_multi_char("\x1b[8m");
    }
    
    void reset() {
        _cpy_multi_char("\x1b[0m");
    }
    void write_string(size_t r, size_t c, const char * str) {
        char fmt_buf[32];
        snprintf(fmt_buf, 32, "\x1b[%d;%dH%7s", y + r * 2 + 1, x + c * 10 + 1, str);
        _cpy_multi_char(fmt_buf);
        // _cpy_multi_char(str);
    }


    void write_double(size_t r, size_t c, double d) {
        char fmt_buf[32];
        snprintf(fmt_buf, 32, "\x1b[%d;%dH%7.3f", y + r * 2 + 1, x + c * 10 + 1, d);
        _cpy_multi_char(fmt_buf);
    }

    void draw() {
        if(i == 0) return;
        // _cpy_multi_char("\x1b[H");
        reset();
        buffer[i] = '\0';
        fputs(buffer, stdout);
        i = 0;
    }
};

struct cli_menu {

};

namespace cli {
    void move_to();
}

cli_table acceleration_disp;
cli_table gyro_disp;

cli_table orientation_disp;

cli_table gps_disp;
cli_table press_disp;
cli_table magnet_disp;

bool running = true;

char last  = '?';
char last_n = -1;

void input_thread() {
    while(running) {
        char inp[256];
        
        int n = read(0, inp, 256);

        // write(1, inp, n);

        last = inp[0];
        last_n = n;

    }
}



void print_action(const char * msg) {
    fputs(msg, stdout);
}
void print_success(const char * msg = "OK") {
    printf("\x1b[1;32m%s\x1b[0m", msg);
} 
void print_fail(const char * msg = "FAILED") {
    printf("\x1b[1;31m%s\x1b[0m", msg);
}

bool adiru_connected = false;

void tick() {
    acceleration_disp.write_double(1, 1, mpu6050_data[0]);
    acceleration_disp.write_double(1, 2, mpu6050_data[1]);
    acceleration_disp.write_double(1, 3, mpu6050_data[2]);
    acceleration_disp.write_double(2, 1, filtered_mpu6050_data[0]);
    acceleration_disp.write_double(2, 2, filtered_mpu6050_data[1]);
    acceleration_disp.write_double(2, 3, filtered_mpu6050_data[2]);
    
    gyro_disp.write_double(1, 1, mpu6050_data[3] * RAD_TO_DEG);
    gyro_disp.write_double(1, 2, mpu6050_data[4] * RAD_TO_DEG);
    gyro_disp.write_double(1, 3, mpu6050_data[5] * RAD_TO_DEG);
    gyro_disp.write_double(2, 1, filtered_mpu6050_data[3] * RAD_TO_DEG);
    gyro_disp.write_double(2, 2, filtered_mpu6050_data[4] * RAD_TO_DEG);
    gyro_disp.write_double(2, 3, filtered_mpu6050_data[5] * RAD_TO_DEG);

    orientation_disp.write_double(1, 0, euler_angles[0] * RAD_TO_DEG);
    orientation_disp.write_double(1, 1, euler_angles[1] * RAD_TO_DEG);
    orientation_disp.write_double(1, 2, euler_angles[2] * RAD_TO_DEG);

    gps_disp.write_double(1, 0, gps_data[0]);
    gps_disp.write_double(1, 1, gps_data[1]);

    press_disp.write_double(1, 0, bmp390_data[0]);
    press_disp.write_double(1, 1, bmp390_data[1] / 1000);
    press_disp.write_double(1, 2, bmp390_data[2]);

    magnet_disp.write_double(1, 0, qmc5883_data[0]);
    magnet_disp.write_double(1, 1, qmc5883_data[1]);
    magnet_disp.write_double(1, 2, qmc5883_data[2]);

    acceleration_disp.draw();
    gyro_disp.draw();
    orientation_disp.draw();
    gps_disp.draw();
    press_disp.draw();
    magnet_disp.draw();

    logger::draw();

    // fputs("\x1b[50;10%HLast char: ", stdout);
    // fputc(last, stdout);
    // printf("\x1b[50;10HLast char: %s N: %d", &last, last_n);
    print_action("\x1b[2;100HADIRU - ");
    if(adiru_connected) print_success("ONLINE ");
    else print_fail("OFFLINE");
    print_action("\x1b[3;102HGPS - ");
    if(gps_data[2] > 0) print_success("FIXED ");
    else print_fail("NO FIX");

    fputs("\x1b[H", stdout);
    
    fflush(stdout);

    return;
    // draw_power_bar("FL Motor", 50, 0.1);
    // draw_power_bar("FR Motor", 50, 0.3);
    // draw_power_bar("BL Motor", 50, 0.2);
    // draw_power_bar("BR Motor", 50, 0.4);

    // fflush(stdout);
}

void print_loop() {
    timer::create_timer(tick, 1000 / 10);    
}


termios old_terminal_settings;

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_terminal_settings);
    fputs("\x1b[?1049l", stdout);
    fflush(stdout);
}


void on_interrupt(int signum) {
    disable_raw_mode();
    std::exit(1);
}

void enable_raw_mode() {
    termios raw;
    tcgetattr(STDIN_FILENO, &old_terminal_settings);
    raw = old_terminal_settings;
    raw.c_lflag &= ~(ECHO | ICANON);

    atexit(disable_raw_mode);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void connect_adiru(int socket) {
    sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, "/tmp/adiru.sock", sizeof(addr.sun_path));

    while(!adiru_connected) {
        if(connect(socket, (sockaddr *) &addr, sizeof(addr)) < 0) {
            usleep(100000);
        }else {
            adiru_connected = true;
        }
    }
}

int main() {

    signal(SIGINT, on_interrupt);
    enable_raw_mode();
    // fputs("\x1b[?1049h", stdout);
    fputs("\x1b[?1049h\x1b[H\x1b[2J", stdout);
    fflush(stdout);

    acceleration_disp.x = 10;
    gyro_disp.x = 54;
    acceleration_disp.y = 2;
    gyro_disp.y = 2;

    orientation_disp.x = 45;
    orientation_disp.y = 12;

    press_disp.x = 10;
    press_disp.y = 12;

    gps_disp.x = 10;
    gps_disp.y = 24;

    magnet_disp.x = 10;
    magnet_disp.y = 18;

    {
        acceleration_disp.init(3, 4);

        acceleration_disp.set_bold();

        acceleration_disp.write_string(0, 1, "ax");
        acceleration_disp.write_string(0, 2, "ay");
        acceleration_disp.write_string(0, 3, "az");

        acceleration_disp.write_string(1, 0, "raw");
        acceleration_disp.write_string(2, 0, "filter");
        
        acceleration_disp.reset();
    }

    {
        gyro_disp.init(3, 4);
        
        gyro_disp.set_bold();

        gyro_disp.write_string(0, 1, "vr");
        gyro_disp.write_string(0, 2, "vp");
        gyro_disp.write_string(0, 3, "vy");

        gyro_disp.write_string(1, 0, "raw");
        gyro_disp.write_string(2, 0, "filter");

        gyro_disp.reset();
    }

    {
        orientation_disp.init(2, 3);

        orientation_disp.set_bold();
        // orientation_disp.set_underline();

        orientation_disp.write_string(0, 0, "roll");
        orientation_disp.write_string(0, 1, "pitch");
        orientation_disp.write_string(0, 2, "yaw");

        orientation_disp.reset();
    }

    {
        press_disp.init(2, 3);

        press_disp.set_bold();

        press_disp.write_string(0, 0, "temp");
        press_disp.write_string(0, 1, "press");
        press_disp.write_string(0, 2, "altitude");

        press_disp.reset();
    }

    {
        gps_disp.init(2, 4);
        
        gps_disp.set_bold();

        gps_disp.write_string(0, 0, "Lat");
        gps_disp.write_string(0, 1, "Long");
        gps_disp.write_string(0, 2, "disp x");
        gps_disp.write_string(0, 3, "disp y");

        gps_disp.reset();
    }

    {
        magnet_disp.init(2, 3);
        
        magnet_disp.set_bold();

        magnet_disp.write_string(0, 0, "mag x");
        magnet_disp.write_string(0, 1, "mag y");
        magnet_disp.write_string(0, 2, "mag z");

        magnet_disp.reset();
    }
    
    std::thread t(print_loop);
    std::thread t2(input_thread);


    int fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    
    // logger::log("Info one, info 2, info 3");
    connect_adiru(fd);
    
    while(1) {
        int n = recv(fd, (char*) data, sizeof(data), 0);
        if(n <= 0) {
            logger::log("Lost connection to the ADIRU");
            adiru_connected = false;
            close(fd);
            fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
            connect_adiru(fd);
        }
    }


}