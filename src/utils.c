#include "ft_ping.h"

double get_timestamp(void) {
    struct timeval tv;
    if (gettimeofday(&tv, NULL) < 0) {
        perror("gettimeofday");
        return 0;
    }
    return (double)tv.tv_sec * 1000.0 + (double)tv.tv_usec / 1000.0;
}

void print_usage(void) {
    printf("Usage: ft_ping [-v] [-?] <destination>\n");
    printf("\n");
    printf("Options:\n");
    printf("  -v          verbose output\n");
    printf("  -?          give this help list\n");
}

void error_exit(const char *msg) {
    if (errno) {
        fprintf(stderr, "ft_ping: %s: %s\n", msg, strerror(errno));
    } else {
        fprintf(stderr, "ft_ping: %s\n", msg);
    }
    exit(1);
}
