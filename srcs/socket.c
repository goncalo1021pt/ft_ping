#include "ft_ping.h"

int create_icmp_socket() {
	int sockfd;
	
	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0) {
		if (errno == EPERM || errno == EACCES) {
			fprintf(stderr, "ft_ping: Operation not permitted\n");
			fprintf(stderr, "ft_ping: You need root privileges to create raw sockets\n");
			fprintf(stderr, "ft_ping: Try running with sudo\n");
		} else {
			perror("ft_ping: socket creation failed");
		}
		return -1;
	}
	
	return sockfd;
}

void close_ping_socket(int sockfd) {
	if (sockfd >= 0) {
		close(sockfd);
	}
}

int resolve_address(t_ping *ping) {
	struct hostent *host_entry;
	
	memset(&ping->dest_addr, 0, sizeof(ping->dest_addr));
	ping->dest_addr.sin_family = AF_INET;
	
	if (inet_aton(ping->ip_address, &ping->dest_addr.sin_addr) != 0) {
		ping->hostname = ping->ip_address;
		printf("PING %s (%s)\n", ping->hostname, ping->ip_address);
		return 0;
	} else {
		host_entry = gethostbyname(ping->ip_address);
		if (host_entry == NULL) {
			fprintf(stderr, "ft_ping: %s: Name or service not known\n", ping->ip_address);
			return -1;
		}
		memcpy(&ping->dest_addr.sin_addr, host_entry->h_addr_list[0], host_entry->h_length);
		ping->hostname = ping->ip_address;
		printf("PING %s (%s)\n", ping->hostname, inet_ntoa(ping->dest_addr.sin_addr));
		return 0;
	}
}