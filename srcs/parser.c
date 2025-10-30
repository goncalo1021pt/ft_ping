#include "ft_ping.h"

void initialize_program(t_options *options, t_ping *ping) {
	options->count = -1;
	options->interval = 1.0;
	options->verbose = false;
	options->help = false;
	
	options->flood = false;
	options->preload = 1;
	options->numeric = false;

	options->deadline = 0;
	options->timeout = 100;
	
	options->pattern = NULL;
	options->packet_size = 64;
	
	options->timestamp_type = 0;
	options->ttl = 64;
	options->ip_timestamp = false;
	
	ft_bzero(ping, sizeof(t_ping));
}

void print_options(t_options *options) {
	printf("=== Parsed Options ===\n");
	printf("Basic:\n");
	printf("  Count: %d\n", options->count);
	printf("  Interval: %.1f seconds\n", options->interval);
	printf("  Verbose: %s\n", options->verbose ? "true" : "false");
	printf("  Help: %s\n", options->help ? "true" : "false");
	
	printf("Network:\n");
	printf("  Flood: %s\n", options->flood ? "true" : "false");
	printf("  Preload: %d\n", options->preload);
	printf("  Numeric: %s\n", options->numeric ? "true" : "false");
	
	printf("Timing:\n");
	printf("  Deadline: %d seconds\n", options->deadline);
	printf("  Timeout: %d seconds\n", options->timeout);
	
	printf("Packet:\n");
	printf("  Pattern: %s\n", options->pattern ? options->pattern : "(default)");
	printf("  Packet size: %d bytes\n", options->packet_size);
	
	printf("Advanced:\n");
	printf("  Timestamp type: %d\n", options->timestamp_type);
	printf("  TTL: %d\n", options->ttl);
	printf("===================\n");
}

void print_help() {
	printf("\nUsage\n");
	printf("  ft_ping [options] <destination>\n\n");
	printf("Options:\n");
	printf("  <destination>      dns name or ip address\n");
	printf("  -c <count>         stop after <count> replies\n");
	printf("  -f                 flood ping\n");
	printf("  -h                 print help and exit\n");
	printf("  -i <interval>      seconds between sending each packet\n");
	printf("  -l <preload>       send <preload> number of packages while waiting replies\n");
	printf("  -n                 no dns name resolution\n");
	printf("  -p <pattern>       contents of padding byte\n");
	printf("  -s <size>          use <size> as number of data bytes to be sent\n");
	printf("  -t <ttl>           define time to live\n");
	printf("  -v                 verbose output\n");
	printf("  -w <deadline>      reply wait <deadline> in seconds\n");
	printf("  -W <timeout>       time to wait for response\n");
	printf("\nIPv4 options:\n");
	printf("  -T <timestamp>     define timestamp, can be one of <tsonly|tsandaddr|tsprespec>\n");
	printf("\nFor more details see ft_ping(8).\n");
}

int parse_options(int argc, char **argv, t_ping *ping, t_options *options) {
	int opt;
	
	if (argc < 2) {
		fprintf(stderr, "ft_ping: usage error: Destination address required\n");
		return 1; 
	}
	while ((opt = getopt(argc, argv, "c:i:hvfl:np:rs:t:w:W:T:")) != -1) {
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
				if (options->interval < 0.02)
					options->interval = 0.02;
				break;
			case 'h':
				options->help = true;
				return 2; 
			case 'v':
				options->verbose = true;
				break;
			case 'f':
				options->flood = true;
				break;
			case 'l':
				if (ft_strspn(optarg, "0123456789") != strlen(optarg) || strlen(optarg) == 0) {
					fprintf(stderr, "ft_ping: invalid argument: '%s'\n", optarg);
					return 2;
				}
				options->preload = atoi(optarg);
				if (options->preload < 1 || options->preload > 65536) {
					fprintf(stderr, "ft_ping: invalid argument: '%d' out of range: 1 <= value <= 65536\n", options->preload);
					return 2;
				}
				break;
			case 'n':
				options->numeric = true;
				break;
			case 'p':
				options->pattern = optarg;	
				break;
			case 's':
				options->packet_size = atoi(optarg);
				if (options->packet_size < 0 || options->packet_size > 65507) {
					fprintf(stderr, "ft_ping: packet size out of range\n");
					return 2;
				}
				break;
			case 't':
				options->ttl = atoi(optarg);
				if (options->ttl < 1 || options->ttl > 255) {
					fprintf(stderr, "ft_ping: ttl out of range (1-255)\n");
					return 2;
				}
				break;
			case 'w':
				options->deadline = atoi(optarg);
				if (options->deadline < 0) {
					fprintf(stderr, "ft_ping: deadline must be non-negative\n");
					return 2;
				}
				break;
			case 'W':
				options->timeout = atoi(optarg);
				if (options->timeout < 0) {
					fprintf(stderr, "ft_ping: timeout must be non-negative\n");
					return 2;
				}
				break;
			case 'T':
				if (strcmp(optarg, "tsonly") == 0) {
					options->timestamp_type = 1;
				} else if (strcmp(optarg, "tsandaddr") == 0) {
					options->timestamp_type = 2;
				} else if (strcmp(optarg, "tsprespec") == 0) {
					options->timestamp_type = 3;
				} else {
					fprintf(stderr, "ft_ping: invalid timestamp type\n");
					return 2;
				}
				break;
			case '?':
				fprintf(stderr, "\n");
				options->help = true;
				return 2;
			default:
				fprintf(stderr, "ft_ping: usage error: Destination address required\n");
				return 1;
		}
	}
	
	if (optind >= argc) {
		fprintf(stderr, "ft_ping: usage error: Destination address required\n");
		return 1; 
	}
	ping->ip_address = argv[optind];
	return 0;
}