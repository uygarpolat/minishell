/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   functions.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 14:48:00 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/14 17:30:14 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"
#include "../../includes/signals.h"

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

// int	waiting(int pid)
// {
// 	int	status;
// 	int	code;

// 	status = 0;
// 	waitpid(pid, &status, 0);
// 	if (WIFEXITED(status))
// 		code = WEXITSTATUS(status);
// 	else
// 		code = EXIT_FAILURE + g_signal;
// 	return (code);
// }

int	waiting(int pid)
{
	int	status;
	int	code;

	code = 0;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
	{
		code = WEXITSTATUS(status);
		printf("Incoming code: %d (g_signal: %d)\n", code, g_signal);
	}
	else if (WIFSIGNALED(status))
	{
		int sig = WTERMSIG(status);
		// Typical Bash-like practice: 128 + signal_number
		code = 128 + sig;
		printf("Child was terminated by signal %d -> code=%d\n", sig, code);
	}
	else
		code = EXIT_FAILURE;

	return (code);
}

int	print_and_return(char *str)
{
	ft_putstr_fd(str, 2);
	return (0);
}
