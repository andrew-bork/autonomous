#include <util/server.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>

static bool chk_bit(short chked, short mask) {
    return (chked & mask) == mask;
}

static char msg[512];

broadcast_server::broadcast_server() {}
broadcast_server::broadcast_server(const char * path, int backlog) {
    init(path, backlog);
}

void broadcast_server::init(const char * path, int backlog) {
    // Socket address.
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path));
    
    int success = -1;
    
    // Make sure the socket doesnt exist.
    if(access(path, F_OK) == 0) {
        success = unlink(path);
        if(success < 0) {
            snprintf(msg, 512, "Failed to unlink previous socket at \"%s\" : ", path);
            perror(msg);
            exit(-1);
        }
    }

    // Create socket.
    server_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if(server_fd < 0) {
        snprintf(msg, 512, "Failed to create socket at \"%s\" : ", path);
        perror(msg);
        exit(-1);
    }

    // Bind socket.
    success = bind(server_fd, (sockaddr *) &addr, sizeof(addr));
    if(success < 0) {
        snprintf(msg, 512, "Failed to bind socket at \"%s\" : ", path);
        perror(msg);
        exit(-1);
    }

    // Set socket to listen mode.
    success = listen(server_fd, backlog);
    if(success < 0) {
        snprintf(msg, 512, "Listen failed; Socket at \"%s\" : ", path);
        perror(msg);
        exit(-1);
    }

    // Create epoll struct.
    epoll_fd = epoll_create(1);
    if(epoll_fd < 0) {
        snprintf(msg, 512, "Epoll struct creation failed; Socket at \"%s\" : ", path);
        perror(msg);
        exit(-1);
    }


    // Add server to epoll struct to listen for connections.
    epoll_event evt;
    evt.events = EPOLLIN;
    evt.data.fd = server_fd;

    success = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &evt);
    if(success < 0) {
        snprintf(msg, 512, "Failed to add epoll event; Socket at \"%s\" : ", path);
        perror(msg);
        exit(-1);
    }
}

broadcast_server::~broadcast_server() {
    clean_up();
}

void broadcast_server::clean_up() {
    for(auto i = listeners.begin(); i != listeners.end(); i ++) {
        close(*i);
    }
    if(epoll_fd >= 0) close(epoll_fd);
    if(server_fd >= 0) close(server_fd);

    if(access(addr.sun_path, F_OK) == 0) {
        int success = unlink(addr.sun_path);
        if(success < 0) {
            snprintf(msg, 512, "Failed to unlink socket at \"%s\" : ", addr.sun_path);
            perror(msg);
        }
    }

}

void broadcast_server::broadcast() {
    // Loop through and send data.
    for(auto i = listeners.begin(); i != listeners.end(); i ++) {
        size_t n_sent = send((*i), data, data_size, 0);
        if(n_sent != data_size) {
            perror("Failed to send data : ");
            exit(-1);
        }
    }
}

void broadcast_server::handle_events() {

    epoll_event events[10];
    int n_events = epoll_wait(epoll_fd, events, 10, 0);   
    
    if(n_events < 0) {
        perror("Failed to poll the polling structure :");
        exit(-1);
    } 
    
    // Loop through events.
    for(int i = 0; i < n_events; i ++) {
        // Check if the event is from the server socket.
        if((events[i].data.fd == server_fd) && chk_bit(events[i].events, EPOLLIN)) {
            // Accept the pending connection.
            int client_fd = accept(server_fd, NULL, NULL);
            if(client_fd < 0) {
                perror("Failed to accept client : ");
                exit(-1);
            }

            // Add it to the list of listeners
            listeners.insert(client_fd);
            
            // Add checking the client for hang up to the polling structure.
            {
                epoll_event evt;
                evt.events = EPOLLHUP | EPOLLRDHUP;
                evt.data.fd = client_fd;
                int success = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &evt);
                if(success < 0) {
                    perror("Failed to add client to polling structure : ");
                    exit(-1);
                }
            }
        }else {
            // Remove the client from the list of listeners
            listeners.erase(events[i].data.fd);
            
            // Remove the client from the polling structure.
            int success = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
            if(success < 0) {
                perror("Failed to remove client from polling structure :");
                exit(-1);
            }

            // Close the client file descriptor.
            close(events[i].data.fd);
        }
    }

}

void broadcast_server::tick() {
    handle_events();
    broadcast();
}