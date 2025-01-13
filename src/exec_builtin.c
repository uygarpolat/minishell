/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 14:34:26 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/13 14:49:37 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"

int	only_builtin(char ***envp, t_minishell *minishell, t_put *cmd)
{
	if (!execute_builtin(minishell->ast, envp, minishell, 0, cmd))
	{	
		error(minishell, cmd, envp);
		return (1);
	}
	if (cmd->infile)
	{
		close(STDIN_FILENO);
		dup2(cmd->stdin2, STDIN_FILENO);
		close(cmd->stdin2);
	}
	if (cmd->outfile)
	{
		close(STDOUT_FILENO);
		dup2(cmd->stdout2, STDOUT_FILENO);
		close(cmd->stdout2);
	}
	close_and_free(&minishell->p, cmd);
	if (minishell->p.pids)
		free(minishell->p.pids);
	if (access(".heredoc", F_OK))
		unlink(".heredoc");
	return (0);
}

void	run_builtin(t_ast *s, char ***envp, t_minishell *minishell, int n, t_put *cmd)
{
	if (!execute_builtin(s, envp, minishell, n, cmd))
	{
		error(minishell, cmd, envp);
		//can i exit or should i just return???
		exit(1);
	}
	//clean everything
	//if (minishell->p.count > 0) //do we need this???
	free_2d_array((void ***)envp);
	free_ast(&minishell->ast);
	free(minishell->p.pids);
	close_and_free(&minishell->p, cmd);
	free_tokens(minishell->tokens, &minishell->capacity);
	//can i exit or should i just return???
	exit(0);
}
