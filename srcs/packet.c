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
	bytes_sent = sendto(ping->sockfd, &packet, sizeof(packet), 0, (struct sockaddr*)&ping->dest_addr, sizeof(ping->dest_addr));
	if (bytes_sent < 0) {
		perror("ft_ping: sendto failed");
		return;
	}
	if (bytes_sent != sizeof(packet))
		printf("ft_ping: Warning - only sent %zd of %zu bytes\n",  bytes_sent, sizeof(packet));

	ping->stats.packets_sent++;
	printf("64 bytes sent to %s: icmp_seq=%d\n", ping->hostname, ping->sequence + 1);
}

void resolve_packet(t_ping *ping, t_options *options) {
	(void)options;
	t_ping_packet packet;

	create_icmp_packet(&packet, ping->identifier, ping->sequence);
	send_packet(ping, &packet);
	ping->sequence++;
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