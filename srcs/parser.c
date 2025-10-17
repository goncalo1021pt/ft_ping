#include "ft_ping.h"

void initialize_options(t_options *options) {
	options->count = -1;
	options->verbose = false;
	options->help = false;
}

void print_options(t_options *options) {
	printf("Options:\n");
	printf("  Count: %d\n", options->count);
	printf("  Verbose: %s\n", options->verbose ? "true" : "false");
	printf("  Help: %s\n", options->help ? "true" : "false");
}

void print_help() {
	printf("Usage: ft_ping [options] <destination>\n");
	printf("Options:\n");
	printf("  -c <count>    Number of echo requests to send\n");
	printf("  -v            Verbose output\n");
	printf("  -h            Display this help message\n");
}

int parse_options(int argc, char **argv, t_ping *ping, t_options *options) {
	int opt;
	
	while ((opt = getopt(argc, argv, "c:hv")) != -1) {
		switch (opt) {
			case 'c':
				if (strspn(optarg, "0123456789") != strlen(optarg) || strlen(optarg) == 0) {
                    fprintf(stderr, "ft_ping: invalid argument: '%s'\n", optarg);
                    options->help = true;
                    return 2;
                }
				options->count = atoi(optarg); 
				break;
			case 'h':
				options->help = true;
				return 2; 
			case 'v':
				options->verbose = true;
				break;
			case '?':
				options->help = true;
				return 2;
			default:
				return 1;
		}
	}
	
	if (optind >= argc) {
		return 1; 
	}
	
	ping->index = optind;
	return 0;
}