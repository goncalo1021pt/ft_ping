#include "ft_ping.h"

int create_icmp_socket(float interval, int timeout) {
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

	int timeout_sec = 0;
	int timeout_usec = 0;
	if (timeout > 0) {
		timeout_sec = timeout;
		timeout_usec = 0;
	} else {
		timeout_sec = (int)interval;
		timeout_usec = (int)((interval - timeout_sec) * 1000000);
		timeout_usec += 200000;
		if (timeout_usec >= 1000000) {
			timeout_sec += timeout_usec / 1000000;
			timeout_usec = timeout_usec % 1000000;
		}
	}

	int flags = fcntl(sockfd, F_GETFL, 0);
	if (flags != -1) {
		fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
	}

	struct timeval tv = {timeout_sec, timeout_usec};
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == -1) {
		perror("ft_ping: setsockopt SO_RCVTIMEO");
	}
	int rcvbuf = 64 * 1024;
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf)) == -1) {
		perror("ft_ping: setsockopt SO_RCVBUF");
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
		printf("PING %s (%s) %d(%d) bytes of data.\n", ping->hostname, ping->ip_address, DATA_SIZE, PACKET_SIZE + 20);
		return 0;
	} else {
		host_entry = gethostbyname(ping->ip_address);
		if (host_entry == NULL) {
			fprintf(stderr, "ft_ping: %s: Name or service not known\n", ping->ip_address);
			return -1;
		}
		memcpy(&ping->dest_addr.sin_addr, host_entry->h_addr_list[0], host_entry->h_length);
		ping->hostname = ping->ip_address;
		printf("PING %s (%s) %d(%d) bytes of data.\n", ping->hostname, inet_ntoa(ping->dest_addr.sin_addr), DATA_SIZE, PACKET_SIZE + 20);
		return 0;
	}
}