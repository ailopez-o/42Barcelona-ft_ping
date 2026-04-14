#include "ft_ping.h"

int create_raw_socket(void) {
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        if (errno == EACCES || errno == EPERM) {
            fprintf(stderr, "ft_ping: Lacking privilege for raw socket. Are you root?\n");
        } else {
            perror("socket");
        }
        exit(1);
    }
    return sockfd;
}

void set_socket_options(int sockfd, t_ping_args *args) {
    struct timeval tv;
    tv.tv_sec = args->linger;
    tv.tv_usec = 0;

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("setsockopt RCVTIMEO");
    }

    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &args->ttl, sizeof(args->ttl)) < 0) {
        perror("setsockopt IP_TTL");
    }
}
