#define PORT 13284

#include <backend/udp.hpp>
#include <iostream>
#include <sys/types.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>


int main() {
    // hostent entry;
    ifaddrs * addresses;
    int success = getifaddrs(&addresses);
    if(success < 0) {
        throw std::runtime_error("Couldn't get network interface information");
    }
    for(ifaddrs* curr = addresses; curr != NULL; curr = curr->ifa_next) {
        uint32_t ip_address = htonl(((sockaddr_in *) curr->ifa_addr)->sin_addr.s_addr);

        printf("%10s: %15s : 0x%08x\n", curr->ifa_name, inet_ntoa(((sockaddr_in *) curr->ifa_addr)->sin_addr), ip_address);
    }

    udp::socket server;
    server.enable_broadcast();
    server.bind(PORT);

    const char msg[] = "Hello World!";

    std::array<std::uint8_t, sizeof(msg)> out_packet;
    std::array<std::uint8_t, sizeof(msg)> in_packet;
    memcpy(&out_packet[0], msg, sizeof(msg));
    
    while(true) {
        auto client = server.recieve(in_packet);
        server.send(out_packet, client);
    }
}