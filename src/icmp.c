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

void send_icmp_echo(int sockfd, struct sockaddr_in *dest_addr, int seq, pid_t pid) {
    struct icmphdr icmp;
    char packet[PACKET_SIZE];
    
    memset(&icmp, 0, sizeof(icmp));
    icmp.type = ICMP_ECHO;
    icmp.code = 0;
    icmp.un.echo.id = htons(pid & 0xFFFF);
    icmp.un.echo.sequence = htons(seq);
    
    memset(packet, 0, PACKET_SIZE);
    memcpy(packet, &icmp, sizeof(icmp));

    // Add timestamp to payload
    double now = get_timestamp();
    memcpy(packet + sizeof(icmp), &now, sizeof(double));

    // Fill remaining payload with some data (optional padding)
    for (int i = sizeof(icmp) + sizeof(double); i < PACKET_SIZE; i++) {
        packet[i] = i;
    }

    // Calculate checksum over the whole packet
    ((struct icmphdr *)packet)->checksum = calculate_checksum(packet, PACKET_SIZE);

    if (sendto(sockfd, packet, PACKET_SIZE, 0, (struct sockaddr *)dest_addr, sizeof(*dest_addr)) <= 0) {
        perror("sendto");
    }
}

int receive_icmp_reply(int sockfd, t_ping_stats *stats, int verbose, pid_t pid) {
    char buffer[1024];
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
        // Not our packet (maybe from another ping instance)
        return 0;
    }

    if (icmp->type == ICMP_ECHOREPLY) {
        double now = get_timestamp();
        double sent_time;
        memcpy(&sent_time, buffer + ip_hdr_len + sizeof(struct icmphdr), sizeof(double));

        double rtt = now - sent_time;
        update_stats(stats, rtt);

        char src_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(from.sin_addr), src_ip, INET_ADDRSTRLEN);

        printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.2f ms\n",
               (int)(bytes_recv - ip_hdr_len), src_ip, ntohs(icmp->un.echo.sequence), ip->ttl, rtt);
        return 1;
    } else {
        if (verbose) {
            printf("Unexpected ICMP type: %d\n", icmp->type);
        }
    }

    return 0;
}
