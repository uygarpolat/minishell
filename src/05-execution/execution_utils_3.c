/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils_3.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 09:49:12 by upolat            #+#    #+#             */
/*   Updated: 2025/01/29 17:52:22 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"
#include "../../includes/signals.h"

void	put_heredoc_ctrl_d(char *str)
{
	ft_putstr_fd("minishell: warning: here-document at" \
		"line 1 delimited by end-of-file", 2);
	ft_putstr_fd(" (wanted ", 2);
	ft_putstr_fd("`", 2);
	ft_putstr_fd(str, 2);
	ft_putstr_fd("')\n", 2);
}

void	heredoc_echo(int fd, char *buf)
{
	write(fd, buf, ft_strlen(buf));
	write(fd, "\n", 1);
	free_void((void **)&buf, NULL);
}

int	is_numeric(t_ast *s, int j)
{
	int	i;

	i = -1;
	while (s->words[j] && s->words[j][++i])
	{
		if (!ft_isdigit(s->words[j][i]))
		{
			if (i == 0 && (s->words[j][i] == '+' || s->words[j][i] == '-'))
				continue ;
			return (0);
		}
	}
	return (1);
}

void	error_message(t_ast *s)
{
	ft_putstr_fd("minishell: exit: ", 2);
	if (!is_numeric(s, 1))
	{
		ft_putstr_fd(s->words[1], 2);
		ft_putstr_fd(": numeric argument required\n", 2);
	}
	else
		ft_putstr_fd("too many arguments\n", 2);
}
