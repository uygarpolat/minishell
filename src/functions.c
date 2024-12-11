/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   functions.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 14:48:00 by hpirkola          #+#    #+#             */
/*   Updated: 2024/12/04 14:57:28 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"

void	close_and_free(t_pipes *p, t_put *cmd)
{
	int	i;

	i = 0;
	if (p->pipes)
	{
		while (i < p->count)
		{
			close(p->pipes[i][0]);
			close(p->pipes[i][1]);
			free(p->pipes[i]);
			i++;
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
