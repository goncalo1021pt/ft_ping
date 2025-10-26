#include "ft_ping.h"

uint16_t calculate_checksum(void *data, int length) {
	uint16_t *buffer = (uint16_t *)data;
	uint32_t sum = 0;
	while (length > 1) {
		sum += *buffer++;
		length -= 2;
	}
	if (length == 1)
		sum += *(uint8_t *)buffer;
	while (sum >> 16)
		sum = (sum & 0xFFFF) + (sum >> 16);
	return (uint16_t)(~sum);
}

void set_timestamps(t_ping_packet *packet) {
	struct timeval timestamp;
	gettimeofday(&timestamp, NULL);
	memcpy(packet->data, &timestamp, sizeof(struct timeval));
}

void create_icmp_packet(t_ping_packet *packet, uint16_t identifier, uint16_t sequence) {
	packet->header.type = ICMP_ECHO;
	packet->header.code = 0;
	packet->header.identifier = htons(identifier);
	packet->header.sequence = htons(sequence);
	packet->header.checksum = 0;
	set_timestamps(packet);
	for (int i = sizeof(struct timeval); i < DATA_SIZE; i++) 
		packet->data[i] = 0x10 + ((i - sizeof(struct timeval)) % 0x28);
	packet->header.checksum = calculate_checksum(packet, sizeof(t_ping_packet));
}

void send_packet(t_ping *ping, t_ping_packet *packet) {
	ssize_t bytes_sent;
	bytes_sent = sendto(ping->sockfd, packet, sizeof(t_ping_packet), 0, (struct sockaddr*)&ping->dest_addr, sizeof(ping->dest_addr));
	if (bytes_sent < 0) {
		perror("ft_ping: sendto failed");
		return;
	}
	if (bytes_sent != sizeof(t_ping_packet))
		printf("ft_ping: Warning - only sent %zd of %zu bytes\n",  bytes_sent, sizeof(t_ping_packet));

	ping->stats.packets_sent++;
}

void recv_packet(t_ping *ping, t_ping_packet *packet) {
	(void)packet;
	ssize_t bytes_received;
	struct sockaddr_in src_addr;
	unsigned char buf[1024];
	socklen_t addr_len = sizeof(src_addr);

	do {
		addr_len = sizeof(src_addr);
		bytes_received = recvfrom(ping->sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&src_addr, &addr_len);
	} while (bytes_received < 0 && errno == EINTR);

	if (bytes_received < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			printf("Request timed out.\n");
			return;
		}
		perror("ft_ping: recvfrom failed");
		return;
	}

	if (bytes_received < (ssize_t)sizeof(struct iphdr)) {
		printf("ft_ping: packet too small for IP header (%zd bytes)\n", bytes_received);
		return;
	}
	struct iphdr *ip_header = (struct iphdr *)buf;
	int ip_header_len = ip_header->ihl * 4;

	if (bytes_received < ip_header_len + (int)sizeof(t_icmp_header)) {
		printf("ft_ping: packet too small for ICMP header (%zd bytes, need %d)\n", 
		       bytes_received, ip_header_len + (int)sizeof(t_icmp_header));
		return;
	}
	t_icmp_header *icmp_header = (t_icmp_header *)(buf + ip_header_len);
	if (icmp_header->type != ICMP_ECHOREPLY) {
		printf("ft_ping: received ICMP type %d (not echo reply)\n", icmp_header->type);
		return;
	}

	uint16_t recv_id = ntohs(icmp_header->identifier);
	if (recv_id != ping->identifier) {
		printf("ft_ping: identifier mismatch (got %u, expected %u)\n", recv_id, ping->identifier);
		return;
	}

	uint16_t recv_seq = ntohs(icmp_header->sequence);
	if (bytes_received >= ip_header_len + (int)sizeof(t_icmp_header) + (int)sizeof(struct timeval)) {
		struct timeval *sent_time = (struct timeval *)(buf + ip_header_len + sizeof(t_icmp_header));
		struct timeval now;
		gettimeofday(&now, NULL);
		double rtt = (now.tv_sec - sent_time->tv_sec) * 1000.0 + (now.tv_usec - sent_time->tv_usec) / 1000.0;

		ping->stats.total_rtt += rtt;
		if (rtt < ping->stats.min_rtt)
			ping->stats.min_rtt = rtt;
		if (rtt > ping->stats.max_rtt)
			ping->stats.max_rtt = rtt;

		char src_ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(src_addr.sin_addr), src_ip, INET_ADDRSTRLEN);
		ping->stats.packets_received++;
		printf("%zd bytes from %s: icmp_seq=%u ttl=%d time=%.1f ms\n", bytes_received - ip_header_len, src_ip, recv_seq, ip_header->ttl, rtt);
	} else
		printf("ft_ping: packet too small for timestamp payload\n");
}

void resolve_packet(t_ping *ping, t_options *options) {
	(void)options;
	t_ping_packet packet;

	create_icmp_packet(&packet, ping->identifier, ping->sequence);
	ping->sequence++;
	send_packet(ping, &packet);
	recv_packet(ping, &packet);
}

void debug_packet(t_ping_packet *packet) {
	printf("=== ICMP Packet Debug ===\n");
	printf("Type: %d (should be %d)\n", packet->header.type, ICMP_ECHO);
	printf("Code: %d\n", packet->header.code);
	printf("Checksum: 0x%04X\n", ntohs(packet->header.checksum));
	printf("Identifier: %d (network: 0x%04X)\n", ntohs(packet->header.identifier), packet->header.identifier);
	printf("Sequence: %d (network: 0x%04X)\n", ntohs(packet->header.sequence), packet->header.sequence);
	
	printf("Data (first 16 bytes): ");
	for (int ctd = 0; ctd < 16 && ctd < DATA_SIZE; ctd++)
		printf("%02X ", (unsigned char)packet->data[ctd]);
	printf("\n");
	printf("======================\n");
}