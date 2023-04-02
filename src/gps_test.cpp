#include <util/str_utils.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <vector>

#define INTERNAL_BUFFER 1024

enum gps_talkers{
    gps = 0,
    glonass = 1,
    galileo = 2,
    beidou = 3
};
const char * talkers_to_string[] = {"gps", "glonass", "galileo" , "beidou"};




void process_nmea_line(const std::string& nmea_msg) {
    string::tokenizer tokenizer(nmea_msg);
    std::string command = tokenizer.next_token();
    
    if(command[0] != '$') {
        printf("Fuck shit!\n");
        return;
    }
    

    gps_talkers talker;
    if(command[1] == 'G') {
        switch(command[2]) {
        case 'B': // beidou
            talker = beidou;
            break;
        case 'A': // Galileo
            talker = galileo;
            break;
        case 'P': // Navstar GPS
            talker = gps;
            break;
        case 'L': // Glonass
            talker = glonass;
            break;
        default:
            break;
        }
    }else if(command[1] == 'B' && command[2] == 'D') {
        // Beidou
        talker = beidou;
    }else {
        // Unknown
        printf("Unknown talker!\n");
        return;
    }
    
    if(command[3] == 'G') {
        if(command[4] == 'G'){
            if(command[5] == 'A') {
                // printf("(GGA) - %s\n", nmea_msg.c_str());
            

            }
        }else if(command[4] == 'L') {
            if(command[5] == 'L') {
                printf("(GLL) - %s\n", nmea_msg.c_str());
            }
        }else if(command[4] == 'S') {
            if(command[5] == 'A') {
                printf("(GSA) - %s\n", nmea_msg.c_str());
            }
        }
    }else if(command[3] == 'R') {
        if(command[4] == 'M') {
            if(command[5] == 'C') {
                printf("(RMC) - %s\n", nmea_msg.c_str());
            }
        }
    }else if(command[3] == 'V') {
        if(command[4] == 'T') {
            if(command[5] == 'G') {
                printf("(VTG) - %s\n", nmea_msg.c_str());
            }
        }
    }
    // if(nmea_args[0].rfind("GGA", 3)) {
    //     // Global Positioning System Fixed Data 
    // }else if(nmea_args[0].rfind("GLL", 3)) {
    //     // Geographic Position-- Latitude and Longitude 
    // }else if(nmea_args[0].rfind("GSA", 3)) {
    //     // GNSS DOP and active satellites 
    // }else if(nmea_args[0].rfind("GSV", 3)) {
    //     // GNSS satellites in view 
    // }else if(nmea_args[0].rfind("RMC", 3)) {
    //     // Recommended minimum specific GPS data
    // }else if(nmea_args[0].rfind("VTG", 3)) {
    //     // Course over ground and ground speed 
    // }else {
    //     // Unknown
    //     return;
    // }

}

int main() {
    int fd = open("/dev/serial0", O_RDWR|O_SYNC);
    
    char buf[INTERNAL_BUFFER+1];
    std::string current = "";


    int len = 0;
    int chg_len = read(fd, buf, INTERNAL_BUFFER);
    buf[chg_len]='\0';
    printf("%s\n", buf);

    

    while(true) {
        int chg_len = read(fd, buf, INTERNAL_BUFFER);
        if(chg_len <= 2) {
            chg_len = read(fd, buf, INTERNAL_BUFFER);
        }
        buf[chg_len-1]='\0';
        current = std::string(buf);
        process_nmea_line(buf);
        // printf("%s\n", buf);
        read(fd, buf, INTERNAL_BUFFER);
    }
}