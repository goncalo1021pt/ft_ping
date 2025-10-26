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

	set_signal();
	termios_change(false);

	ping->identifier = getpid();
	ping->sequence = 1;
	
	ping->stats.packets_sent = 0;
	ping->stats.packets_received = 0;
	ping->stats.min_rtt = 999999.0;
	ping->stats.max_rtt = 0.0;
	ping->stats.total_rtt = 0.0;
	gettimeofday(&ping->stats.start_time, NULL);
	
	if (resolve_address(ping) < 0) 
		return -1;
	
	ping->sockfd = create_icmp_socket(options->interval, options->timeout);
	if (ping->sockfd < 0)
		return -1; 
	
	g_alarm_received = true;
	return 0;
}

void print_statistics_exit(t_ping *ping) {
	struct timeval now;
	gettimeofday(&now, NULL);
	long total_time_ms = (now.tv_sec - ping->stats.start_time.tv_sec) * 1000 + (now.tv_usec - ping->stats.start_time.tv_usec) / 1000;

	double packet_loss = 0.0;
	if (ping->stats.packets_sent > 0) {
		packet_loss = ((double)(ping->stats.packets_sent - ping->stats.packets_received) / ping->stats.packets_sent) * 100.0;
	}
	printf("--- %s ping statistics ---\n", ping->ip_address);
	printf("%d packets transmitted, %d received, %.0f%% packet loss, time %ldms\n",
	       ping->stats.packets_sent, ping->stats.packets_received, packet_loss, total_time_ms);

	if (ping->stats.packets_received > 0) {
		double avg_rtt = ping->stats.total_rtt / ping->stats.packets_received;
		double mdev = (ping->stats.max_rtt - ping->stats.min_rtt) / 4.0;
		printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",ping->stats.min_rtt, avg_rtt, ping->stats.max_rtt, mdev);
	}
}

void print_statistics(t_ping *ping) {
	double packet_loss = 0.0;
	if (ping->stats.packets_sent > 0) {
		packet_loss = ((double)(ping->stats.packets_sent - ping->stats.packets_received) / ping->stats.packets_sent) * 100.0;
	}

	if (ping->stats.packets_received > 0) {
		double avg_rtt = ping->stats.total_rtt / ping->stats.packets_received;
		double ewma_rtt = avg_rtt; 
		printf("%d/%d packets, %.0f%% loss, min/avg/ewma/max = %.3f/%.3f/%.3f/%.3f ms\n",
		       ping->stats.packets_received, ping->stats.packets_sent, packet_loss,
		       ping->stats.min_rtt, avg_rtt, ewma_rtt, ping->stats.max_rtt);
	} else {
		printf("%d/%d packets, %.0f%% loss\n",
		       ping->stats.packets_received, ping->stats.packets_sent, packet_loss);
	}
}

int start_ping_loop(t_ping *ping, t_options *options) {

	set_interval_timer(options->interval);
	
	while (g_ping_running) {
		if (g_alarm_received && g_ping_running) {
			g_alarm_received = false;
			resolve_packet(ping, options);
			set_interval_timer(options->interval);
		} else if (g_statistics_requested) {
			g_statistics_requested = false;
			print_statistics(ping);
		}
		pause();
	}
	set_interval_timer(0);
	return 0;
}

int cleanup_ping(t_ping *ping, t_options *options) {
	(void)options;
	
	print_statistics_exit(ping);
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
