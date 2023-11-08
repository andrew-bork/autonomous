#include <backend/udp.hpp>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>

udp::socket::socket() {
    fd = ::socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0) {
        throw udp::posix_error("Couldn't create a socket.");
    }
}

// void udp::socket::send(const)


void udp::socket::broadcast(const std::span<const std::uint8_t> buffer, short port) {

    sockaddr_in info = {0};
    info.sin_family = AF_INET;
    info.sin_addr.s_addr = INADDR_BROADCAST;
    info.sin_port = htons(port);
    
    ssize_t result = sendto(fd, &buffer[0], buffer.size(), 0, (sockaddr *) &info, sizeof(info));
    if(result < 0) {
        throw udp::posix_error("There was an error when sending to a socket");
    }
}

void udp::socket::send(const std::span<const std::uint8_t> buffer, const udp::address_port& destination) {
    sockaddr_in info = {0};
    info.sin_family = AF_INET;
    info.sin_addr.s_addr = destination.address.addr_int32;
    info.sin_port = htons(destination.port);
    
    ssize_t result = sendto(fd, &buffer[0], buffer.size(), 0, (sockaddr *) &info, sizeof(info));
    if(result < 0) {
        throw udp::posix_error("There was an error when sending to a socket");
    }
}

udp::address_port udp::socket::recieve(std::span<std::uint8_t> buffer) {
    udp::address_port connection_info;
    sockaddr_in info = {0};
    socklen_t info_size;
    ssize_t result = recvfrom(fd, &buffer[0], buffer.size(), 0, (sockaddr *) &info, &info_size);

    connection_info.address.addr_int32 = info.sin_addr.s_addr;
    connection_info.port = ntohs(info.sin_port);

    if(result < 0) {
        throw udp::posix_error("There was an error when recieving data");
    }

    return connection_info;
}

void udp::socket::bind(std::uint16_t port) {
    sockaddr_in info = {0};
    info.sin_family = AF_INET;
    info.sin_addr.s_addr = INADDR_ANY;
    info.sin_port = htons(port);
    ssize_t result = ::bind(fd, (sockaddr *) &info, sizeof(info));
    if(result < 0) {
        throw udp::posix_error("There was an error when binding to a port");
    }
}

void udp::socket::enable_broadcast() {
    int enable = 1;
    int success = setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &enable, sizeof(enable));
    if(success < 0) {
        throw udp::posix_error("There was an error when enabling broadcast");
    }
}


udp::socket::~socket() {
    close(fd);
}

std::string udp::posix_error::get_error() {
    std::string out = "";
    out += std::strerror(errno);
    return out;
}