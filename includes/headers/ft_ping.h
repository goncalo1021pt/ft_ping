#ifndef FT_PING_H
# define FT_PING_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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
}				t_ping;


// parser.c
void initialize_options(t_options *options);
void print_options(t_options *options);
void print_help();
int parse_options(int argc, char **argv, t_ping *ping, t_options *options) ;

#endif