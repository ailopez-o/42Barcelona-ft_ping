#include "ft_ping.h"

unsigned short calculate_checksum(void *addr, int len) {
    unsigned long sum = 0;
    unsigned short *ptr = (unsigned short *)addr;

    while (len > 1) {
        sum += *ptr++;
        len -= 2;
    }

    if (len == 1) {
        sum += *(unsigned char *)ptr;
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);

    return (unsigned short)(~sum);
}

void send_icmp_echo(int sockfd, struct sockaddr_in *dest_addr, int seq, pid_t pid, int payload_size) {
    int packet_size = sizeof(struct icmphdr) + payload_size;
    char *packet = malloc(packet_size);
    if (!packet) {
        perror("malloc");
        return;
    }

    struct icmphdr *icmp = (struct icmphdr *)packet;
    memset(packet, 0, packet_size);
    
    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->un.echo.id = htons(pid & 0xFFFF);
    icmp->un.echo.sequence = htons(seq);
    
    // Add timestamp to payload if there's enough space
    if (payload_size >= (int)sizeof(double)) {
        double now = get_timestamp();
        memcpy(packet + sizeof(struct icmphdr), &now, sizeof(double));
    }

    // Fill remaining payload with some data
    int start_fill = sizeof(struct icmphdr) + (payload_size >= (int)sizeof(double) ? sizeof(double) : 0);
    for (int i = start_fill; i < packet_size; i++) {
        packet[i] = i;
    }

    // Calculate checksum
    icmp->checksum = calculate_checksum(packet, packet_size);

    if (sendto(sockfd, packet, packet_size, 0, (struct sockaddr *)dest_addr, sizeof(*dest_addr)) <= 0) {
        perror("sendto");
    }
    free(packet);
}

int receive_icmp_reply(int sockfd, t_ping_stats *stats, int verbose, pid_t pid) {
    char buffer[65536]; // Max IP packet size
    struct sockaddr_in from;
    socklen_t from_len = sizeof(from);
    ssize_t bytes_recv;

    bytes_recv = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&from, &from_len);
    if (bytes_recv < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0; // Timeout
        }
        perror("recvfrom");
        return -1;
    }

    struct iphdr *ip = (struct iphdr *)buffer;
    int ip_hdr_len = ip->ihl * 4;

    if (bytes_recv < ip_hdr_len + (int)sizeof(struct icmphdr)) {
        if (verbose) fprintf(stderr, "Packet too short\n");
        return -1;
    }

    struct icmphdr *icmp = (struct icmphdr *)(buffer + ip_hdr_len);

    // Validate ID
    if (ntohs(icmp->un.echo.id) != (pid & 0xFFFF)) {
        return 0;
    }

    if (icmp->type == ICMP_ECHOREPLY) {
        double now = get_timestamp();
        double rtt = -1.0;
        
        // Only calculate RTT if we have the timestamp in the payload
        int icmp_payload_len = bytes_recv - ip_hdr_len - sizeof(struct icmphdr);
        if (icmp_payload_len >= (int)sizeof(double)) {
            double sent_time;
            memcpy(&sent_time, buffer + ip_hdr_len + sizeof(struct icmphdr), sizeof(double));
            rtt = now - sent_time;
            update_stats(stats, rtt);
        }

        char src_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(from.sin_addr), src_ip, INET_ADDRSTRLEN);

        printf("%ld bytes from %s: icmp_seq=%d ttl=%d",
               bytes_recv - ip_hdr_len, src_ip, ntohs(icmp->un.echo.sequence), ip->ttl);
        if (rtt >= 0) {
            printf(" time=%.2f ms\n", rtt);
        } else {
            printf("\n");
        }
        return 1;
    } else {
        if (verbose) {
            printf("Unexpected ICMP type: %d\n", icmp->type);
        }
    }

    return 0;
}
