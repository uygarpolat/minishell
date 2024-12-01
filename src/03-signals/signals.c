/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 00:35:18 by upolat            #+#    #+#             */
/*   Updated: 2024/12/01 19:05:37 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/signals.h"


/*
void	handle_sigint(int signum)
{
	(void)signum;
	g_signal = 1;
	ft_putstr_fd("\n", STDOUT_FILENO);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	handle_sigquit(int signum)
{
	rl_on_new_line();
	rl_redisplay();
	(void)signum;
}
*/






//int	event(void);

static void	set_signal_exit(int *exit_code)
{
	//static t_ms	*ms_struct;

	if (!exit_code)
	{
		*exit_code = 130;
	}
	//else
	//	ms_struct = ms;
}

static void	sig_handler_heredoc(int signo)
{
	if (signo == SIGINT)
	{
		ft_putstr_fd("\n", STDOUT_FILENO);
		rl_on_new_line();
		rl_replace_line("", 0);
		//rl_done = 1;
		set_signal_exit(NULL);
	}
}

static void	sig_parent_handler(int signo)
{
	if (signo == SIGINT)
	{
		ft_putstr_fd("\n", STDOUT_FILENO);
		if (!isatty(STDIN_FILENO) || errno != EINTR)
			return ;
		//set_signal_exit(NULL);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

static int	set_parent(void)
{
	struct sigaction	sa;

	//rl_done = 0;
	//rl_event_hook = NULL;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = sig_parent_handler;
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGINT, &sa, NULL) == -1)
		return (0);
	if (signal(SIGQUIT, SIG_IGN) == SIG_ERR)
		return (0);
	return (1);
}

int	set_signals(int *exit_code, int type)
{
	set_signal_exit(exit_code);
	//ms->abort = 1;
	if (type == SIGNAL_PARENT)
	{
		if (!set_parent())
			return (0);
	}
	else if (type == SIGNAL_HEREDOC)
	{
		//rl_event_hook = event;
		if (signal(SIGINT, sig_handler_heredoc) == SIG_ERR)
			return (0);
		if (signal(SIGQUIT, SIG_IGN) == SIG_ERR)
			return (0);
	}
	else if (type == SIGNAL_CHILD)
	{
		if (signal(SIGINT, SIG_DFL) == SIG_ERR)
			return (0);
		if (signal(SIGQUIT, SIG_DFL) == SIG_ERR)
			return (0);
	}
	//ms->abort = 0;
	return (1);
}




int	init_term_and_signal(int argc, char **argv, int *exit_code)
{
	struct termios	term;

	(void)argc;
	(void)argv;
	if (isatty(STDIN_FILENO))
	{
		if (tcgetattr(STDIN_FILENO, &term) == -1)
			return (perror("tcgetattr"), 1);
		term.c_lflag &= ~ECHOCTL;
		if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1)
			return (perror("tcsetattr"), 1);
	}

	/*
	if (signal(SIGINT, handle_sigint) == SIG_ERR)
		return (perror("signal SIGINT"), 1);
	if (signal(SIGQUIT, handle_sigquit) == SIG_ERR)
		return (perror("signal SIGQUIT"), 1);
	*/
	set_signals(exit_code, SIGNAL_PARENT);
	return (0);
}
