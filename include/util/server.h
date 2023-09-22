#pragma once

#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/un.h>
#include <unordered_set>

struct broadcast_server {
    
    // Set this to the data buffer
    char * data = NULL;
    size_t data_size = 0;

    sockaddr_un addr;
    int server_fd = -1;
    int epoll_fd = -1;
    std::unordered_set<int> listeners;

    broadcast_server();
    broadcast_server(const char * path, int backlog_n = 8);
    ~broadcast_server();

    void init(const char * path, int backlog_n = 8);
    void clean_up();

    void handle_events();
    void broadcast();
    void tick();
};

