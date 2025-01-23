/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 11:05:16 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/23 19:29:42 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"

void	error(t_minishell *minishell, t_put *cmd)
{
	free_2d_array((void ***)minishell->envp);
	if (minishell->p.pipes)
		close_and_free(&minishell->p, cmd, 0);
	else
	{
		free_heredocs(cmd);
		free(cmd->cmd_fd);
	}
	if (minishell->p.pids)
		free(minishell->p.pids);
	if (minishell->tokens)
		free_tokens(&minishell->tokens, &minishell->capacity);
}

void	file_error(t_minishell *minishell, t_put *cmd)
{
	free_2d_array((void ***)minishell->envp);
	if (minishell->p.pipes)
		close_and_free(&minishell->p, cmd, 0);
	else
	{
		free_heredocs(cmd);
		free(cmd->cmd_fd);
	}
	if (minishell->p.pids)
		free(minishell->p.pids);
	if (minishell->ast)
		free_ast(&minishell->ast);
	if (minishell->tokens)
		free_tokens(&minishell->tokens, &minishell->capacity);
}

void	error2(t_minishell *minishell, char *str, t_put *cmd)
{
	ft_putchar_fd(' ', 2);
	ft_putstr_fd(str, 2);
	if (minishell->p.pipes)
		close_and_free(&minishell->p, cmd, 0);
	if (minishell->p.pids)
		free(minishell->p.pids);
	if (minishell->ast)
		free_ast(&minishell->ast);
	if (minishell->tokens)
	{
		free_tokens(&minishell->tokens, &minishell->capacity);
		minishell->tokens = NULL;
	}
}

int	print_error(char *str)
{
	ft_putstr_fd(str, 2);
	ft_putstr_fd(": ", 2);
	ft_putstr_fd(strerror(errno), 2);
	ft_putchar_fd('\n', 2);
	return (0);
}

void	print_and_exit(char *cmd, char *str, int code, t_minishell *minishell)
{
	if (!cmd || !cmd[0])
		ft_putstr_fd("''", 2);
	else
		ft_putstr_fd(cmd, 2);
	ft_putstr_fd(": ", 2);
	ft_putstr_fd(str, 2);
	if (minishell->ast)
		free_ast(&minishell->ast);
	if (minishell->tokens)
		free_tokens(&minishell->tokens, &minishell->capacity);
	exit(code);
}
