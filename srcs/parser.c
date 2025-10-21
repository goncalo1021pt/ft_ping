#include "ft_ping.h"

void initialize_options(t_options *options) {
	options->count = -1;
	options->interval = 1;
	options->verbose = false;
	options->help = false;
}

void print_options(t_options *options) {
	printf("Options:\n");
	printf("Count: %d\n", options->count);
	printf("Interval: %.1f seconds\n", options->interval);
	printf("Verbose: %s\n", options->verbose ? "true" : "false");
	printf("Help: %s\n", options->help ? "true" : "false");
}

void print_help() {
	printf("Usage: ft_ping [options] <destination>\n");
	printf("Options:\n");
	printf("  -c <count>    Number of echo requests to send\n");
	printf("  -i <interval> Wait interval seconds between sending each packet\n");
	printf("  -v            Verbose output\n");
	printf("  -h            Display this help message\n");
}

int parse_options(int argc, char **argv, t_ping *ping, t_options *options) {
	int opt;
	
	while ((opt = getopt(argc, argv, "c:i:hv")) != -1) {
		switch (opt) {
			case 'c':
				if (ft_strspn(optarg, "0123456789") != strlen(optarg) || strlen(optarg) == 0) {
                    fprintf(stderr, "ft_ping: invalid argument: '%s'\n", optarg);
                    options->help = true;
                    return 2;
                }
				options->count = atoi(optarg); 
				break;
			case 'i':
				if (!ft_is_valid_decimal(optarg)) {
                    fprintf(stderr, "ft_ping: invalid interval: '%s'\n", optarg);
                    options->help = true;
                    return 2;
                }
				options->interval = atof(optarg);
				if (options->interval <= 0) {
					fprintf(stderr, "ft_ping: interval must be greater than 0\n");
					options->help = true;
					return 2;
				}
				break;
			case 'h':
				options->help = true;
				return 2; 
			case 'v':
				options->verbose = true;
				break;
			case '?':
				fprintf(stderr, "\n");
				options->help = true;
				return 2;
			default:
				return 1;
		}
	}
	
	if (optind >= argc) {
		return 1; 
	}
	ping->ip_address = argv[optind];
	ping->index = optind;
	return 0;
}