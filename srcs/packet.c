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

	bytes_sent = sendto(ping->sockfd, packet, sizeof(t_ping_packet), 0,(struct sockaddr*)&ping->dest_addr, sizeof(ping->dest_addr));
	if (bytes_sent < 0) {
		perror("ft_ping: sendto failed");
		return;
	}
	if (bytes_sent != (ssize_t)sizeof(t_ping_packet))
		printf("ft_ping: Warning - only sent %zd of %zu bytes\n",  bytes_sent, sizeof(t_ping_packet));

	ping->stats.packets_sent++;
	printf("64 bytes sent to %s: icmp_seq=%d\n", ping->hostname, ping->sequence + 1);
}

void recv_packet(t_ping *ping, t_ping_packet *packet) {
	ssize_t bytes_received;
	unsigned char buf[1024];
	struct sockaddr_in src_addr;
	socklen_t addr_len = sizeof(src_addr);
	struct timeval tv = {1, 0}; 
	(void)packet;
	
	setsockopt(ping->sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	ft_printf("Waiting to receive packet...\n");
	bytes_received = recvfrom(ping->sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&src_addr, &addr_len);
	if (bytes_received < 0) {
		perror("ft_ping: recvfrom failed");
		return;
	}
	ft_printf("Received packet!\n");

	/* parse IP header */
	struct iphdr *ip = (struct iphdr *)buf;
	int ip_header_len = ip->ihl * 4;

	/* sanity check */
	if (bytes_received < ip_header_len + (int)sizeof(struct icmphdr)) {
		printf("ft_ping: received packet too small (%zd bytes)\n", bytes_received);
		return;
	}

	struct icmphdr *icmp = (struct icmphdr *)(buf + ip_header_len);

	/* check for echo reply */
	if (icmp->type != ICMP_ECHOREPLY) {
		printf("ft_ping: received non-echo reply (type=%d code=%d)\n", icmp->type, icmp->code);
		return;
	}

	/* verify identifier (compare in host order)
	   icmp->un.echo.id is in network byte order on the wire, so convert */
	uint16_t recv_id = ntohs(icmp->un.echo.id);
	if (recv_id != ping->identifier) {
		printf("ft_ping: id mismatch (got=%u expected=%u)\n", recv_id, ping->identifier);
		return;
	}

	/* extract send timestamp from ICMP payload and compute RTT */
	struct timeval sent_time, now;
	void *payload = (void *)(buf + ip_header_len + sizeof(struct icmphdr));
	memcpy(&sent_time, payload, sizeof(struct timeval));
	gettimeofday(&now, NULL);
	double rtt = (now.tv_sec - sent_time.tv_sec) * 1000.0 + (now.tv_usec - sent_time.tv_usec) / 1000.0;

	char src_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(src_addr.sin_addr), src_ip, INET_ADDRSTRLEN);

	ping->stats.packets_received++;
	printf("%zd bytes from %s: icmp_seq=%u ttl=%d time=%.3f ms\n",
		   bytes_received - ip_header_len, src_ip, ntohs(icmp->un.echo.sequence), ip->ttl, rtt);
}

void resolve_packet(t_ping *ping, t_options *options) {
	(void)options;
	t_ping_packet packet;

	create_icmp_packet(&packet, ping->identifier, ping->sequence);
	send_packet(ping, &packet);
	ping->sequence++;

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