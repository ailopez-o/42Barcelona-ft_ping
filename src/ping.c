#include "ft_ping.h"

void start_ping(t_ping_state *state) {
    printf("PING %s (%s): %d data bytes\n", state->args.dest, state->ip_str, state->args.payload_size);

    double start_total = get_timestamp();
    int seq = 0;
    while (!state->stop) {
        // Check global deadline
        if (state->args.deadline > 0) {
            double now = get_timestamp();
            if (now - start_total >= state->args.deadline * 1000.0) {
                break;
            }
        }

        send_icmp_echo(state->sockfd, &state->dest_addr, seq++, state->pid, state->args.payload_size);
        state->stats.packets_sent++;

        double start_send = get_timestamp();
        while (!state->stop) {
            // Check global deadline inside receive loop as well
            if (state->args.deadline > 0) {
                double now = get_timestamp();
                if (now - start_total >= state->args.deadline * 1000.0) {
                    state->stop = 1;
                    break;
                }
            }

            int ret = receive_icmp_reply(state->sockfd, &state->stats, state->args.verbose, state->pid);
            if (ret == 1) {
                double now = get_timestamp();
                if (now - start_send < 1000.0) {
                    usleep((1000.0 - (now - start_send)) * 1000.0);
                }
                break;
            } else if (ret == 0) {
                double now = get_timestamp();
                if (now - start_send >= 1000.0) {
                    break;
                }
            } else {
                break;
            }
        }
    }
}
