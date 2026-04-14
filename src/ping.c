#include "ft_ping.h"

void start_ping(t_ping_state *state) {
    printf("PING %s (%s): %d data bytes\n", state->args.dest, state->ip_str, PAYLOAD_SIZE);

    int seq = 0;
    while (!state->stop) {
        send_icmp_echo(state->sockfd, &state->dest_addr, seq++, state->pid);
        state->stats.packets_sent++;

        double start_time = get_timestamp();
        while (!state->stop) {
            int ret = receive_icmp_reply(state->sockfd, &state->stats, state->args.verbose, state->pid);
            if (ret == 1) {
                // Reply received, wait for the next second mark
                double now = get_timestamp();
                if (now - start_time < 1000.0) {
                    usleep((1000.0 - (now - start_time)) * 1000.0);
                }
                break;
            } else if (ret == 0) {
                // Timeout or unrelated packet
                double now = get_timestamp();
                if (now - start_time >= 1000.0) {
                    break; // Go to next send
                }
            } else {
                // Error
                break;
            }
        }
    }
}
