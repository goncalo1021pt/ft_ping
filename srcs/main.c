#include "ft_ping.h"

int main(int argc, char **argv)
{
	t_options options;
	t_ping ping;
	int exit_code;

	initialize_program(&options, &ping);
	exit_code = parse_options(argc, argv, &ping, &options);
	if (exit_code == 1) {
		return 2;
	}
	if (options.help || exit_code == 2) {
		if (options.help)
			print_help();
		return 2;
	}
	return exec_ping(&ping, &options);
}	