#include "ft_ping.h"

void parse_args(int argc, char **argv, t_ping_args *args) {
    int opt;
    int option_index = 0;

    // Initialize defaults
    args->verbose = 0;
    args->help = 0;
    args->dest = NULL;
    args->linger = 1;      // Default 1s
    args->deadline = 0;    // Default no deadline
    args->payload_size = 56;
    args->ttl = DEFAULT_TTL;

    static struct option long_options[] = {
        {"ttl", required_argument, 0, 0},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "v?W:w:s:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 0:
                if (strcmp(long_options[option_index].name, "ttl") == 0) {
                    args->ttl = atoi(optarg);
                    if (args->ttl < 1 || args->ttl > 255) {
                        fprintf(stderr, "ft_ping: invalid TTL: %s\n", optarg);
                        exit(1);
                    }
                }
                break;
            case 'v':
                args->verbose = 1;
                break;
            case '?':
                args->help = 1;
                print_usage();
                exit(0);
            case 'W':
                args->linger = atoi(optarg);
                if (args->linger < 0) {
                    fprintf(stderr, "ft_ping: invalid linger: %s\n", optarg);
                    exit(1);
                }
                break;
            case 'w':
                args->deadline = atoi(optarg);
                if (args->deadline < 0) {
                    fprintf(stderr, "ft_ping: invalid deadline: %s\n", optarg);
                    exit(1);
                }
                break;
            case 's':
                args->payload_size = atoi(optarg);
                if (args->payload_size < 0 || args->payload_size > 65399) {
                    fprintf(stderr, "ft_ping: invalid packet size: %s\n", optarg);
                    exit(1);
                }
                break;
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
    hints.ai_family = AF_INET;
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
