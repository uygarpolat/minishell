/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 00:39:48 by upolat            #+#    #+#             */
/*   Updated: 2025/01/13 17:21:12 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/signals.h"

static void	set_signal_exit(int *exit_code)
{
	static int	*exit_code_ptr;

	if (exit_code != NULL)
		exit_code_ptr = exit_code;
	else
		*exit_code_ptr = 130;
}

static void	sig_handler_heredoc(int signo)
{
	if (signo == SIGINT)
	{
		ft_putstr_fd("\n", STDOUT_FILENO);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_done = 1;
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
		set_signal_exit(NULL);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

static int	set_parent(void)
{
	struct sigaction	sa;

	rl_done = 0;
	rl_event_hook = NULL;
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
	if (type == SIGNAL_PARENT)
	{
		if (!set_parent())
			return (0);
	}
	else if (type == SIGNAL_HEREDOC)
	{
		rl_event_hook = event;
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
	return (1);
}
