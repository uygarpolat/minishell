/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/27 13:29:41 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/27 16:38:42 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"

int	init_heredocs(t_put *cmd)
{
	int		i;
	char	*index;

	i = -1;
	while (++i < 16)
	{
		index = ft_itoa(i);
		if (!index)
			return (0);
		cmd->heredocs[i] = ft_strjoin(".heredoc", index);
		free(index);
		if (!cmd->heredocs[i])
			return (0);
	}
	return (1);
}

int	initialize(t_put *cmd, t_minishell *minishell,
		t_ast *s, t_token_info *token_info)
{
	int	i;

	cmd->infile = NULL;
	cmd->outfile = NULL;
	cmd->in = -1;
	cmd->out = -1;
	cmd->stdin2 = -1;
	cmd->stdout2 = -1;
	i = -1;
	while (++i < 16)
		cmd->fd_here[i] = -1;
	minishell->ast = s;
	minishell->p.count = count_pipes(minishell->ast);
	minishell->tokens = token_info->tokens;
	minishell->capacity = token_info->capacity;
	minishell->envp = token_info->envp;
	minishell->here = 0;
	s->code = 0;
	if (!init_heredocs(cmd))
		return (0);
	cmd->cmd_fd = malloc(sizeof(int) * (minishell->p.count + 1));
	if (!cmd->cmd_fd)
		return (0);
	return (1);
}
