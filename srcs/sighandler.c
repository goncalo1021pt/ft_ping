#include "ft_ping.h"

bool g_ping_running = true;

void signal_handler(int signal, siginfo_t *info, void *context)
{
	(void)info;
	(void)context;
	if (signal == SIGINT)
	{
		g_ping_running = false;
	}
	else if (signal == SIGQUIT)
	{
		printf("Received SIGQUIT\n");
	}
	else if (signal == SIGALRM)	
	{
		printf("Received SIGALRM\n");
	}
}

void set_signal()
{
	struct sigaction sa;

	sa.sa_sigaction = signal_handler;
	sa.sa_flags = SA_SIGINFO;
	if (sigemptyset(&sa.sa_mask) != 0)
		return ;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
	sigaction(SIGALRM, &sa, NULL);
}