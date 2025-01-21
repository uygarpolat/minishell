/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   functions.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 14:48:00 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/21 08:23:32 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"
#include "../../includes/signals.h"

void	close_loop(t_pipes *p)
{
	int	i;

	i = 0;
	while (i < 2)
	{
		if (p->pipes[i][0] >= 0)
			close(p->pipes[i][0]);
		if (p->pipes[i][1] >= 0)
			close(p->pipes[i][1]);
		free(p->pipes[i]);
		i++;
	}
}

void	close_and_free(t_pipes *p, t_put *cmd)
{
	if (p->pipes)
	{
		if (p->count == 1)
		{
			close(p->pipes[0][0]);
			close(p->pipes[0][1]);
			free(p->pipes[0]);
		}
		else
			close_loop(p);
		free(p->pipes);
		p->pipes = NULL;
	}
	if (cmd->infile && cmd->in >= 0)
		close(cmd->in);
	if (cmd->outfile && cmd->out >= 0)
		close(cmd->out);
}

int	waiting(int pid)
{
	int	status;
	int	code;

	code = 0;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		code = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		code = 128 + WTERMSIG(status);
	else
		code = EXIT_FAILURE;
	return (code);
}

int	print_and_return(char *str, char *cmd, char *str2)
{
	ft_putstr_fd(str, 2);
	ft_putstr_fd(cmd, 2);
	ft_putstr_fd(str2, 2);
	return (0);
}
