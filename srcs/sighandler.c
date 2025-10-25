#include "ft_ping.h"

volatile sig_atomic_t g_ping_running = true;
volatile sig_atomic_t g_alarm_received = false;
volatile sig_atomic_t g_statistics_requested = false;

void signal_handler(int signal, siginfo_t *info, void *context)
{
	(void)info;
	(void)context;
	if (signal == SIGINT)
	{
		g_ping_running = false; 
		write(STDOUT_FILENO, "^C\n", 3);
	}
	else if (signal == SIGQUIT)
	{
		g_statistics_requested = true;
	}
	else if (signal == SIGALRM)	
	{
		g_alarm_received = true;
	}
}

void set_signal()
{
	struct sigaction sa;

	sa.sa_sigaction = signal_handler;
	sa.sa_flags = SA_SIGINFO | SA_RESTART;
	if (sigemptyset(&sa.sa_mask) != 0)
		return ;
	
	// if (sigaction(SIGINT, &sa, NULL) == -1)
	// 	perror("sigaction SIGINT");
	if (sigaction(SIGQUIT, &sa, NULL) == -1)
		perror("sigaction SIGQUIT");
    if (sigaction(SIGALRM, &sa, NULL) == -1)
        perror("sigaction SIGALRM");
}

int	termios_change(bool echo_ctl_chr)
{
	struct termios	terminos_p;
	int				status;

	status = tcgetattr(STDOUT_FILENO, &terminos_p);
	if (status == -1)
		return (ERROR);
	if (echo_ctl_chr)
		terminos_p.c_lflag |= ECHOCTL;
	else
		terminos_p.c_lflag &= ~(ECHOCTL);
	status = tcsetattr(STDOUT_FILENO, TCSANOW, &terminos_p);
	if (status == -1)
		return (ERROR);
	return (0);
}