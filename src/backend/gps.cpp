#include <backend/gps.h>


void process_nmea_line(const std::string& nmea_msg, nmea_result& result) {
    result.useful = false;

    string::tokenizer tokenizer(nmea_msg, ',');
    std::string command = tokenizer.next_token();

    if(command[0] != '$') {
        // printf("Unknown Command\n");
        return;
    }
    
    {
        // gps_talkers talker;
        // if(command[1] == 'G') {
        //     switch(command[2]) {
        //     case 'B': // beidou
        //         talker = beidou;
        //         break;
        //     case 'A': // Galileo
        //         talker = galileo;
        //         break;
        //     case 'P': // Navstar GPS
        //         talker = gps;
        //         break;
        //     case 'L': // Glonass
        //         talker = glonass;
        //         break;
        //     default:
        //         break;
        //     }
        // }else if(command[1] == 'B' && command[2] == 'D') {
        //     // Beidou
        //     talker = beidou;
        // }else {
        //     // Unknown
        //     printf("Unknown talker!\n");
        //     return;
        // }
        
    }
    
    {
        // if(command[3] == 'G') {
        //     if(command[4] == 'G'){
        //         if(command[5] == 'A') {
        //             // printf("(GGA) - %s\n", nmea_msg.c_str());

        //         }
        //     }else if(command[4] == 'L') {
        //         if(command[5] == 'L') {
        //             // printf("(GLL) - %s\n", nmea_msg.c_str());
        //         }
        //     }else if(command[4] == 'S') {
        //         if(command[5] == 'A') {
        //             // printf("(GSA) - %s\n", nmea_msg.c_str());
        //         }
        //     }
        // }else if(command[3] == 'R') {
        //     if(command[4] == 'M') {
        //         if(command[5] == 'C') {
        //             // printf("(RMC) - %s\n", nmea_msg.c_str());
        //         }
        //     }
        // }else if(command[3] == 'V') {
        //     if(command[4] == 'T') {
        //         if(command[5] == 'G') {
        //             // printf("(VTG) - %s\n", nmea_msg.c_str());
        //         }
        //     }
        // }
    }


    if(command.find("GGA", 3) == 3) {
        // Global Positioning System Fixed Data 
        // printf("%s\n", nmea_msg);
        // https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_GGA.html
        std::string data = tokenizer.next_token(); // UTC of position fix.
        
        data = tokenizer.next_token(); // Latitude
        int deg = std::stoi(data.substr(0,2));
        double min = std::stod(data.substr(2));
        
        data = tokenizer.next_token(); // N/S (Latitude)
        result.north = data[0] == 'N';

        result.lat = deg + min / 60;
        if(!result.north) result.lat *= -1;

        data = tokenizer.next_token(); // Longitude
        deg = std::stoi(data.substr(0,3));
        min = std::stod(data.substr(3));
        
        data = tokenizer.next_token(); // E/W (Longitude)
        result.east = data[0] == 'E';

        result.lng = deg + min / 60;
        if(!result.east) result.lng *= -1;

        data = tokenizer.next_token(); // GPS quality indicator (0 - no fix, 1 - fix, don't worry abt anything else)
        if(data[0] == '0') result.fix = false;
        else result.fix = true;
        
        data = tokenizer.next_token(); // # of satellites used to calculate
        result.n_sat = std::stoi(data);
        
        data = tokenizer.next_token(); // HDOP
        data = tokenizer.next_token(); // Orthometric height (MSL reference)

        result.useful = true;
    }
    
    {
    // else if(command.rfind("GLL", 3)) {
    //     // Geographic Position-- Latitude and Longitude 
    // }else if(command.rfind("GSA", 3)) {
    //     // GNSS DOP and active satellites 
    // }else if(command.rfind("GSV", 3)) {
    //     // GNSS satellites in view 
    // }else if(command.rfind("RMC", 3)) {
    //     // Recommended minimum specific GPS data
    // }else if(command.rfind("VTG", 3)) {
    //     // Course over ground and ground speed 
    // }else {
    //     // Unknown
    //     return;
    // }
    }
}

void gps::init() {
    fd = open("/dev/serial0", O_RDONLY);
    if(fd == -1) return;
    // printf("fdasjkfasdklfj\n");
    thread = std::thread(gps::gps_thread, this);
}

void gps::gps_thread(gps* gps_dev) {
    
    // printf("kms\n");
    char buf[GPS_INTERNAL_BUFFER+1];
    std::string current = "";


    int len = 0;
    int chg_len = read(gps_dev->fd, buf, GPS_INTERNAL_BUFFER);
    buf[chg_len]='\0';
    // printf("%s\n", buf);


    nmea_result result;

    while(1) {
        try {
            int chg_len = read(gps_dev->fd, buf, GPS_INTERNAL_BUFFER);
            if(chg_len <= 2) {
                chg_len = read(gps_dev->fd, buf, GPS_INTERNAL_BUFFER);
            }
            buf[chg_len-1]='\0';
            current = std::string(buf);
            // printf("%s\n", current.c_str());
            process_nmea_line(buf, result);

            if(result.useful) {
                gps_dev->last_useful_nmea_result = result;
                gps_dev->lat = result.lat;
                gps_dev->lng = result.lng;
                gps_dev->fix = result.fix;
                gps_dev->new_data = true;

                // printf("%f, %f\n", gps_dev->lat, gps_dev->lng);
            }

            // read(gps_dev->fd, buf, GPS_INTERNAL_BUFFER);
        }catch(std::exception e) {

        }
    }
}