#include "ft_ping.h"

void set_interval_timer(float interval) {
	struct itimerval timer;
	
	timer.it_value.tv_sec = (long)interval;
	timer.it_value.tv_usec = (long)((interval - (long)interval) * 1000000);
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 0;
	setitimer(ITIMER_REAL, &timer, NULL);
}

int setup_ping(t_ping *ping, t_options *options) {
	(void)options;

	set_signal();
	termios_change(false);

	ping->identifier = getpid();
	ping->sequence = 0;
	
	if (resolve_address(ping) < 0) 
		return -1;
	
	ping->sockfd = create_icmp_socket();
	if (ping->sockfd < 0)
		return -1; 
	
	printf("Socket created successfully (fd: %d)\n", ping->sockfd);
	return 0;
}

void print_statistics() {
	printf("Printing ft_ping statistics...\n");
}

int start_ping_loop(t_ping *ping, t_options *options) {

	set_interval_timer(options->interval);
	while (g_ping_running) {
		pause();
		if (g_alarm_received && g_ping_running) {
			g_alarm_received = false;
			resolve_packet(ping, options);
			set_interval_timer(options->interval);
		} else if (g_statistics_requested) {
			g_statistics_requested = false;
			print_statistics();
		}
	}
	set_interval_timer(0);
	return 0;
}

int cleanup_ping(t_ping *ping, t_options *options) {
	(void)options;
	
	printf("--- %s ft_ping statistics ---\n", ping->ip_address);
	close_ping_socket(ping->sockfd);
	termios_change(true);
	
	return 0;
}

int exec_ping(t_ping *ping, t_options *options)
{
	int exit_code = 0;
	if (setup_ping(ping, options) < 0)
		return -1;  
	start_ping_loop(ping, options);
	cleanup_ping(ping, options);

	return exit_code;
}
