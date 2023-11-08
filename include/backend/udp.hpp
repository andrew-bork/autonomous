#pragma once

#include <cstdlib>
#include <span>
#include <string> 
#include <stdexcept>

namespace udp {

    /**
     * @brief An wrapper for any error throw by POSIX functions. Automatically gets the corresponding POSIX error message.
     * 
     */
    class posix_error : public std::runtime_error {
        public:
            static std::string get_error();

            posix_error(const std::string& msg) : std::runtime_error(msg + " : " + udp::posix_error::get_error()){
            }
    };

    /**
     * @brief A simple struct containing a host address (in host endianness), and port (also in host endianness)
     * 
     */
    struct address_port {
        union {
            std::uint8_t addr_bytes[4];
            std::uint32_t addr_int32;
        } address;
        std::uint16_t port;
    };

    /**
     * @brief A wrapper for a UDP socket. Automatically manages a socket file descriptor.
     * 
     */
    struct socket {

        /**
         * @brief Construct a new socket object
         * @throws A udp::posix_error if an error was encountered during creation.
         */
        socket();

        /**
         * @brief Beginning listening for messages on a certain port. 
         * 
         * @param port Port to listen to. This should be in host endianness. htons will be called on this.
         */
        void bind(std::uint16_t port);

        /**
         * @brief Enable the socket option to send a recieve UDP broadcasts.
         * 
         */
        void enable_broadcast();


        /**
         * @brief Recieve a message from a socket. If the socket is bound, it will recieve a message from the bound port.
         * 
         * @param buffer Recieved datagram
         * @return address_port Information on sender.
         */
        address_port recieve(std::span<std::uint8_t> buffer);

        /**
         * @brief Broadcast a message to a port. You must enable_broadcast() before using this.
         * 
         * @param buffer Datagram to broadcast.
         * @param port Port to broadcast to. Host endianness.
         */
        void broadcast(const std::span<const std::uint8_t> buffer, short port);

        /**
         * @brief Send a datagram to a certain socket.
         * 
         * @param buffer Datagram to send.
         * @param destination Information of datagram recipient
         */
        void send(const std::span<const std::uint8_t> buffer, const udp::address_port& destination);

        /**
         * @brief Destroy the socket object. Closes the associated file descriptor.
         * 
         */
        ~socket();

        private:
            std::size_t fd;
    };
}