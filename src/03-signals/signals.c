/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 00:35:18 by upolat            #+#    #+#             */
/*   Updated: 2024/11/17 15:27:46 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/signals.h"

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
	(void)signum;
}

int	init_term_and_signal(int argc, char **argv)
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
	if (signal(SIGINT, handle_sigint) == SIG_ERR)
		return (perror("signal SIGINT"), 1);
	if (signal(SIGQUIT, handle_sigquit) == SIG_ERR)
		return (perror("signal SIGQUIT"), 1);
	return (0);
}
