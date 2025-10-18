#ifndef FT_PING_H
# define FT_PING_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "../libft/libft.h"

typedef struct s_options
{
	int count;
	bool verbose;
	bool help;
}				t_options;

typedef struct s_ping
{
	int index;
	char *ip_address;
}				t_ping;

extern bool g_ping_running;

// parser.c
void initialize_options(t_options *options);
void print_options(t_options *options);
void print_help();
int parse_options(int argc, char **argv, t_ping *ping, t_options *options) ;

// exec.c
int exec_ping(t_ping *ping, t_options *options);

// sighandler.c
void set_signal();

#endif