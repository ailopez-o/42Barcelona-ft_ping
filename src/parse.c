#include "ft_ping.h"

void parse_args(int argc, char **argv, t_ping_args *args) {
    int opt;

    args->verbose = 0;
    args->help = 0;
    args->dest = NULL;

    while ((opt = getopt(argc, argv, "v?")) != -1) {
        switch (opt) {
            case 'v':
                args->verbose = 1;
                break;
            case '?':
                args->help = 1;
                print_usage();
                exit(0);
            default:
                print_usage();
                exit(1);
        }
    }

    if (optind < argc) {
        args->dest = argv[optind];
    } else {
        fprintf(stderr, "ft_ping: missing host operand\n");
        fprintf(stderr, "Try 'ft_ping -?' for more information.\n");
        exit(1);
    }
}

void resolve_hostname(const char *hostname, struct sockaddr_in *addr, char *ip_str) {
    struct addrinfo hints, *res;
    int status;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4 only
    hints.ai_socktype = SOCK_RAW;

    if ((status = getaddrinfo(hostname, NULL, &hints, &res)) != 0) {
        fprintf(stderr, "ft_ping: unknown host: %s\n", hostname);
        exit(1);
    }

    struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
    *addr = *ipv4;

    if (inet_ntop(AF_INET, &(ipv4->sin_addr), ip_str, INET_ADDRSTRLEN) == NULL) {
        perror("inet_ntop");
        freeaddrinfo(res);
        exit(1);
    }

    freeaddrinfo(res);
}
