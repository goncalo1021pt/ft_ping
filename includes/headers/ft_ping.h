#ifndef FT_PING_H
# define FT_PING_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include "../libft/libft.h"

#define ERROR -1
#define SUCCESS 0

#define PACKET_SIZE 64
#define ICMP_HEADER_SIZE 8
#define DATA_SIZE (PACKET_SIZE - ICMP_HEADER_SIZE)

// ICMP (Internet Control Message Protocol) 
typedef struct s_icmp_header
{
	uint8_t type;
	uint8_t code;
	uint16_t checksum;
	uint16_t identifier;
	uint16_t sequence;
} t_icmp_header;

typedef struct s_ping_packet
{
	t_icmp_header header;
	char data[DATA_SIZE];
} t_ping_packet;

typedef struct s_ping_stats
{
	int packets_sent;
	int packets_received;
	double min_rtt;
	double max_rtt;
	double total_rtt;
	struct timeval start_time;
	struct timeval end_time;
} t_ping_stats;

typedef struct s_options
{
	int count;
	float interval;
	bool verbose;
	bool help;
	
	bool flood;
	int preload;
	bool numeric;
	
	int deadline;
	int timeout;
	
	char *pattern;
	int packet_size;
	
	bool bypass_route;
	int timestamp_type;
	int ttl;
	bool ip_timestamp;
} t_options;

typedef struct s_ping
{
	char *ip_address;
	char *hostname;
	int sockfd;
	struct sockaddr_in dest_addr;
	uint16_t identifier;
	uint16_t sequence;
	t_ping_stats stats;
} t_ping;

extern volatile sig_atomic_t g_ping_running;
extern volatile sig_atomic_t g_alarm_received;
extern volatile sig_atomic_t g_statistics_requested;

// parser.c
void initialize_program(t_options *options, t_ping *ping);
void print_options(t_options *options);
void print_help();
int parse_options(int argc, char **argv, t_ping *ping, t_options *options) ;

// packet.c
void create_icmp_packet(t_ping_packet *packet, uint16_t identifier, uint16_t sequence);
uint16_t calculate_checksum(void *data, int length);
void packet_setup(t_ping *ping, t_options *options);
void debug_packet(t_ping_packet *packet);
void resolve_packet(t_ping *ping, t_options *options);

// socket.c
int create_icmp_socket();
void close_ping_socket(int sockfd);
int resolve_address(t_ping *ping);

// exec.c
void print_statistics();
int exec_ping(t_ping *ping, t_options *options);

// sighandler.c
void set_signal();
int	termios_change(bool echo_ctl_chr);

#endif