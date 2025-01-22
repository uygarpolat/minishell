/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 01:28:07 by upolat            #+#    #+#             */
/*   Updated: 2025/01/22 21:36:12 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNALS_H
# define SIGNALS_H

# include <stdio.h>
# include <termios.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <signal.h>
# include <errno.h>
# include "../library/libft/libft.h"

extern int	g_signal;

enum
{
	SIGNAL_PARENT,
	SIGNAL_CHILD,
	SIGNAL_HEREDOC
};

void	handle_sigint(int signum);
void	handle_sigquit(int signum);
int		init_term_and_signal(int argc, char **argv, int *exit_code);
int		set_signals(int *exit_code, int type);
int		event(void);

#endif
