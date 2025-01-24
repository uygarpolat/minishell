/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils_2.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/24 13:20:01 by upolat            #+#    #+#             */
/*   Updated: 2025/01/24 13:20:55 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"

void	pipe_fail(t_pipes *p)
{
	ft_putstr_fd("Pipe failed\n", 2);
	if (p->count > 1)
		pipe_close(p, 1, 0);
}

int	ft_pipe(t_pipes *p, int n, t_minishell *minishell, t_put *cmd)
{
	if (n % 2 == 0 && n < p->count && p->count > 0)
	{
		if (n > 0)
			pipe_close(p, 0, 0);
		if (pipe(p->pipes[0]) < 0)
			return (error(minishell, cmd), 0);
		if (n > 0)
			close(p->pipes[1][1]);
	}
	else if (n < p->count && p->count > 0)
	{
		if (n > 1)
			pipe_close(p, 1, 0);
		if (pipe(p->pipes[1]) < 0)
		{
			error(minishell, cmd);
			return (pipe_close(p, 0, 1), 0);
		}
		close(p->pipes[0][1]);
	}
	return (1);
}