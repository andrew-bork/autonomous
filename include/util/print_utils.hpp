#include <cstdio>

void print_action(const char * msg) {
    std::printf("%s", msg);
    std::fflush(stdout);
}
void print_success(const char * msg = "OK") {
    std::printf("\x1b[1;32m%s\x1b[0m\n", msg);
} 
void print_fail(const char * msg = "FAILED") {
    std::printf("\x1b[1;31m%s\x1b[0m\n", msg);
}