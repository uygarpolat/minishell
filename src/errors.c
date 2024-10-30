/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 11:05:16 by hpirkola          #+#    #+#             */
/*   Updated: 2024/10/30 12:55:59 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"

/*void	free_ast(t_ast	*ast)
{
	while (ast->left)
		ast = ast->left;
	while (ast)
	{
		if (ast->token->value)
			free(ast->token->value);
		if (ast->redir_target)
			free(ast->redir_target);
		if (ast->left)
			free(ast);
		ast = ast->right;
	}
	if (ast)
		free(ast);
}*/

void	error(t_minishell *minishell, t_command *cmd, char *str)
{
	int	i;

	ft_putstr_fd(cmd->args[0], 2);
	ft_putstr_fd(": ", 2);
	ft_putstr_fd(str, 2);
	if (minishell->ast)
		free_ast(minishell->ast);
	if (minishell->p.pipes)
		close_and_free(&minishell->p);
	if (minishell->p.pids)
		free(minishell->p.pids);
	if (minishell->pwd)
		free(minishell->pwd);
	if (cmd->args)
	{
		i = -1;
		while (cmd->args[++i])
			free(cmd->args[i]);
		free(cmd->args);
	}
	if (cmd->path)
		free(cmd->path);
}
