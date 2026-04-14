#include "ft_ping.h"

t_ping_state g_ping;

void start_ping(t_ping_state *state);

int main(int argc, char **argv) {
    memset(&g_ping, 0, sizeof(g_ping));
    
    // Parse arguments
    parse_args(argc, argv, &g_ping.args);
    
    // Initialize state
    g_ping.pid = getpid();
    init_stats(&g_ping.stats);
    
    // Resolve hostname
    resolve_hostname(g_ping.args.dest, &g_ping.dest_addr, g_ping.ip_str);
    
    // Setup socket
    g_ping.sockfd = create_raw_socket();
    set_socket_options(g_ping.sockfd);
    
    // Setup signal handling
    setup_signals();
    
    // Start pinging
    start_ping(&g_ping);
    
    // Cleanup and final stats
    print_stats(g_ping.args.dest, &g_ping.stats);
    close(g_ping.sockfd);
    
    return 0;
}
