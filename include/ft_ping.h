#ifndef FT_PING_H
# define FT_PING_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <signal.h>
# include <errno.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <math.h>
# include <stdint.h>
# include <getopt.h>

# define PACKET_SIZE 64
# define PAYLOAD_SIZE 56
# define ICMP_HDR_SIZE 8
# define DEFAULT_TTL 64

# ifndef __linux__
// Compatibility structures for non-linux systems (macOS)
struct iphdr {
#  if  __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    unsigned int ihl:4;
    unsigned int version:4;
#  else
    unsigned int version:4;
    unsigned int ihl:4;
#  endif
    uint8_t tos;
    uint16_t tot_len;
    uint16_t id;
    uint16_t frag_off;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t check;
    uint32_t saddr;
    uint32_t daddr;
};

struct icmphdr {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    union {
        struct {
            uint16_t id;
            uint16_t sequence;
        } echo;
        uint32_t gateway;
        struct {
            uint16_t unused_pad;
            uint16_t mtu;
        } frag;
    } un;
};

#  define ICMP_ECHO 8
#  define ICMP_ECHOREPLY 0
# endif

// Argument options
typedef struct s_ping_args {
    char    *dest;
    int     verbose;
    int     help;
    int     linger;       // -W (linger/timeout for reply)
    int     deadline;     // -w (deadline for total execution)
    int     payload_size; // -s (size of data)
    int     ttl;          // --ttl (IP TTL)
} t_ping_args;

// Statistics
typedef struct s_ping_stats {
    int     packets_sent;
    int     packets_received;
    double  min_rtt;
    double  max_rtt;
    double  sum_rtt;
    double  sum_rtt_sq;
} t_ping_stats;

// Global/Main State
typedef struct s_ping_state {
    int                     sockfd;
    t_ping_args             args;
    t_ping_stats            stats;
    struct sockaddr_in      dest_addr;
    char                    ip_str[INET_ADDRSTRLEN];
    pid_t                   pid;
    volatile sig_atomic_t   stop;
} t_ping_state;

extern t_ping_state g_ping;

// Function Prototypes

// parse.c
void    parse_args(int argc, char **argv, t_ping_args *args);
void    resolve_hostname(const char *hostname, struct sockaddr_in *addr, char *ip_str);

// socket.c
int     create_raw_socket(void);
void    set_socket_options(int sockfd, t_ping_args *args);

// icmp.c
void    send_icmp_echo(int sockfd, struct sockaddr_in *dest_addr, int seq, pid_t pid, int payload_size);
int     receive_icmp_reply(int sockfd, t_ping_stats *stats, int verbose, pid_t pid);
unsigned short calculate_checksum(void *addr, int len);

// stats.c
void    init_stats(t_ping_stats *stats);
void    update_stats(t_ping_stats *stats, double rtt);
void    print_stats(const char *dest, t_ping_stats *stats);

// signal.c
void    setup_signals(void);
void    handle_sigint(int sig);

// utils.c
double  get_timestamp(void);
void    print_usage(void);
void    error_exit(const char *msg);

#endif
