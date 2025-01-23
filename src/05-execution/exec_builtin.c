/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 14:34:26 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/23 12:16:16 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"

int	execute_builtin(t_ast *s, t_minishell *minishell, int n, t_put *file)
{
	if (minishell->p.count == 0)
		check_in_out(s, minishell, file, n);
	if (!ft_strncmp(s->words[0], "echo", 5))
		return (run_echo(s->words));
	else if (!ft_strncmp(s->words[0], "cd", 3))
		return (run_cd(s->words, minishell));
	else if (!ft_strncmp(s->words[0], "pwd", 4))
	{
		//printf("%s\n", get_var(*envp, "PWD="));
		printf("%s\n", minishell->pwd);
	}
	else if (!ft_strncmp(s->words[0], "export", 7))
		return (run_export(s->words, minishell->envp));
	else if (!ft_strncmp(s->words[0], "unset", 6))
	{
		if (s->words[1])
			*minishell->envp = rm_envp(*minishell->envp, s->words[1]);
		if (!minishell->envp)
			return (0);
	}
	else if (!ft_strncmp(s->words[0], "env", 4))
		print_env(*minishell->envp);
	else if (!ft_strncmp(s->words[0], "exit", 5))
		return (run_exit(s, minishell, file));
	return (1);
}

int	only_builtin(t_minishell *minishell, t_put *cmd)
{
	if (!execute_builtin(minishell->ast, minishell, 0, cmd))
	{
		free(minishell->p.pids);
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

void	run_builtin(t_ast *s, t_minishell *minishell, int n, t_put *cmd)
{
	if (!execute_builtin(s, minishell, n, cmd))
	{
		error(minishell, cmd);
		exit(1);
	}
	free_2d_array((void ***)minishell->envp);
	free_ast(&minishell->ast);
	free(minishell->p.pids);
	close_and_free(&minishell->p, cmd);
	free_tokens(&minishell->tokens, &minishell->capacity);
	minishell->tokens = NULL;
	//close(0);
	//close(1);
	//close(2);
	exit(0);
}

int	is_builtin(char **cmd)
{
	if (!cmd || !cmd[0])
		return (0);
	if (!ft_strncmp(cmd[0], "echo", 5))
		return (1);
	else if (!ft_strncmp(cmd[0], "cd", 3))
		return (1);
	else if (!ft_strncmp(cmd[0], "pwd", 4))
		return (1);
	else if (!ft_strncmp(cmd[0], "export", 7))
		return (1);
	else if (!ft_strncmp(cmd[0], "unset", 6))
		return (1);
	else if (!ft_strncmp(cmd[0], "env", 4))
		return (1);
	else if (!ft_strncmp(cmd[0], "exit", 5))
		return (1);
	return (0);
}
