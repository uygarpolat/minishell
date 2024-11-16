/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 01:28:07 by upolat            #+#    #+#             */
/*   Updated: 2024/11/16 01:59:50 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNALS_H
# define SIGNALS_H

# include <stdio.h>
# include <termios.h>
# include <readline/readline.h>
# include <signal.h>
# include "../library/libft/libft.h"

extern int	g_signal;

void	handle_sigint(int signum);
void	handle_sigquit(int signum);
int		init_signal(int argc, char **argv);

#endif
