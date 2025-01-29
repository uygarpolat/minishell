/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 00:35:18 by upolat            #+#    #+#             */
/*   Updated: 2025/01/29 16:35:49 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/signals.h"

int	event(void)
{
	return (0);
}

int	init_term_and_signal(int argc, char **argv, int *exit_code)
{
	struct termios	term;

	if (argc > 1)
	{
		*exit_code = 127;
		return (ft_putstr_fd("Usage: ./minishell\n", 2), 1);
	}
	(void)argv;
	if (isatty(STDIN_FILENO))
	{
		if (tcgetattr(STDIN_FILENO, &term) == -1)
			return (perror("tcgetattr"), 1);
		term.c_lflag &= ~ECHOCTL;
		if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1)
			return (perror("tcsetattr"), 1);
	}
	set_signals(exit_code, SIGNAL_PARENT);
	return (0);
}
