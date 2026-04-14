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

void set_socket_options(int sockfd) {
    struct timeval tv;
    tv.tv_sec = 1; // 1 second timeout for receive
    tv.tv_usec = 0;

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("setsockopt RCVTIMEO");
    }

    int ttl = DEFAULT_TTL;
    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
        perror("setsockopt IP_TTL");
    }
}
