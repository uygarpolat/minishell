/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 17:16:21 by upolat            #+#    #+#             */
/*   Updated: 2025/01/22 17:19:13 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"

void	no_words(t_ast *s, t_minishell *minishell, t_put *cmd)
{
	if (!*s->words)
	{
		error(minishell, cmd);
		free_ast(&minishell->ast);
		exit(0);
	}
}

void	pipe_close(t_pipes *p, int pipe_no, int flag)
{
	if (flag)
		ft_putstr_fd("Pipe failed\n", 2);
	close(p->pipes[pipe_no][0]);
	close(p->pipes[pipe_no][1]);
}
