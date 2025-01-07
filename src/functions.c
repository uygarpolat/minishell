/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   functions.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 14:48:00 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/07 14:03:01 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"

void	close_and_free(t_pipes *p, t_put *cmd)
{
	int	i;

	i = 0;
	if (p->pipes)
	{
		if (p->count == 1)
		{
			close(p->pipes[0][0]);
			close(p->pipes[0][1]);
			free(p->pipes[0]);
		}
		else
		{
			while (i < 2)
			{
				close(p->pipes[i][0]);
				close(p->pipes[i][1]);
				free(p->pipes[i]);
				i++;
			}
		}
		free(p->pipes);
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

	status = 0;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		code = WEXITSTATUS(status);
	else
		code = EXIT_FAILURE;
	return (code);
}
