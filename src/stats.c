#include "ft_ping.h"

void init_stats(t_ping_stats *stats) {
    stats->packets_sent = 0;
    stats->packets_received = 0;
    stats->min_rtt = 0.0;
    stats->max_rtt = 0.0;
    stats->sum_rtt = 0.0;
    stats->sum_rtt_sq = 0.0;
}

void update_stats(t_ping_stats *stats, double rtt) {
    if (stats->packets_received == 0 || rtt < stats->min_rtt) {
        stats->min_rtt = rtt;
    }
    if (rtt > stats->max_rtt) {
        stats->max_rtt = rtt;
    }
    stats->packets_received++;
    stats->sum_rtt += rtt;
    stats->sum_rtt_sq += (rtt * rtt);
}

void print_stats(const char *dest, t_ping_stats *stats) {
    printf("--- %s ping statistics ---\n", dest);
    
    double loss = 0.0;
    if (stats->packets_sent > 0) {
        loss = 100.0 * (stats->packets_sent - stats->packets_received) / stats->packets_sent;
    }

    printf("%d packets transmitted, %d received, %.0f%% packet loss\n",
           stats->packets_sent, stats->packets_received, loss);

    if (stats->packets_received > 0) {
        double avg = stats->sum_rtt / stats->packets_received;
        double var = (stats->sum_rtt_sq / stats->packets_received) - (avg * avg);
        double mdev = sqrt(var > 0 ? var : 0);

        printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
               stats->min_rtt, avg, stats->max_rtt, mdev);
    }
}
