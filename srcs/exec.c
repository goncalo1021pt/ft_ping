#include "ft_ping.h"

void print_statistics() {
	printf("Printing ping statistics...\n");
}

int start_ping_loop(t_ping *ping, t_options *options) {
	(void)ping;
	(void)options;

	while (g_ping_running) {
		printf("Pinging %s ...\n", ping->ip_address);
		sleep(1);
	}
	return 0;
}

int setup_ping(t_ping *ping, t_options *options) {
	(void)options;
	(void)ping;
	
	set_signal();
	termios_change(false);
	printf("global ping running set to %d\n", g_ping_running);
	return 0;
}

int cleanup_ping(t_ping *ping, t_options *options) {
	(void)options;
	(void)ping;
	printf("--- %s ping statistics ---\n", ping->ip_address);
	termios_change(true);
	
	return 0;
}

int exec_ping(t_ping *ping, t_options *options)
{
	int exit_code = 0;
	setup_ping(ping, options);
	start_ping_loop(ping, options);
	cleanup_ping(ping, options);

	return exit_code;
}