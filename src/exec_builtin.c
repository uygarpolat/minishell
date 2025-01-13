/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 14:34:26 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/13 16:46:21 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"

int	execute_builtin(t_ast *s, char ***envp, t_minishell *minishell, int n, t_put *file)
{
	if (minishell->p.count == 0)
		check_in_out(s, minishell, file, n);
	if (!ft_strncmp(s->words[0], "echo", 5))
		return (run_echo(s->words));
	else if (!ft_strncmp(s->words[0], "cd", 3))
		return (run_cd(envp, s->words, minishell));
	else if (!ft_strncmp(s->words[0], "pwd", 4))
	{
		//printf("%s\n", get_var(*envp, "PWD="));
		printf("%s\n", minishell->pwd);
	}
	else if (!ft_strncmp(s->words[0], "export", 7))
		return (run_export(s->words, envp));
	else if (!ft_strncmp(s->words[0], "unset", 6))
	{
		if (s->words[1])
			*envp = rm_envp(*envp, s->words[1]);
		if (!envp)
			return (0);
	}
	else if (!ft_strncmp(s->words[0], "env", 4))
		print_env(*envp);
	else if (!ft_strncmp(s->words[0], "exit", 5))
		return (run_exit(s, minishell, file, envp));
	return (1);
}

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
		exit(1);
	}
	free_2d_array((void ***)envp);
	free_ast(&minishell->ast);
	free(minishell->p.pids);
	close_and_free(&minishell->p, cmd);
	free_tokens(minishell->tokens, &minishell->capacity);
	exit(0);
}
