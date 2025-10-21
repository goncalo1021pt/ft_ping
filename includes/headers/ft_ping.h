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
#include "../libft/libft.h"

#define ERROR -1
#define SUCCESS 0

typedef struct s_options
{
	int count;
	int interval;
	bool verbose;
	bool help;
}				t_options;

typedef struct s_ping
{
	int index;
	char *ip_address;
}				t_ping;

extern volatile sig_atomic_t g_ping_running;
extern volatile sig_atomic_t g_alarm_received;

// parser.c
void initialize_options(t_options *options);
void print_options(t_options *options);
void print_help();
int parse_options(int argc, char **argv, t_ping *ping, t_options *options) ;

// exec.c
void print_statistics();
int exec_ping(t_ping *ping, t_options *options);

// sighandler.c
void set_signal();
int	termios_change(bool echo_ctl_chr);

#endif