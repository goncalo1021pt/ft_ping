#include "ft_ping.h"

int main(int argc, char **argv)
{
	t_options options;
	t_ping ping;
	int exit_code;

	initialize_options(&options);
	exit_code = parse_options(argc, argv, &ping, &options);
	if (exit_code == 1) {
		fprintf(stderr, "ft_ping: usage error: Destination address required\n");
		return 1;
	}
	if (options.help || exit_code == 2) {
		print_help();
		return 2;
	}
	// print_options(&options);
	exec_ping(&ping, &options);
	return 0;
}