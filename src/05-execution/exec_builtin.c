/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 14:34:26 by hpirkola          #+#    #+#             */
/*   Updated: 2025/02/03 10:19:04 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"

static int	run_unset(char ***envp, t_ast *s)
{
	int	i;

	if (envp && *envp && **envp && s->words[1])
	{
		i = 0;
		while (s->words[++i])
		{
			*envp = rm_envp(*envp, s->words[i]);
			if (!envp)
				return (0);
		}
	}
	return (1);
}

static int	execute_builtin(t_ast *s, t_minishell *minishell, \
			int n, t_put *file)
{
	if (minishell->p.count == 0)
	{
		if (!check_in_out(s, minishell, file, n))
			return (0);
	}
	if (!ft_strncmp(s->words[0], "echo", 5))
		return (run_echo(s->words));
	else if (!ft_strncmp(s->words[0], "cd", 3))
		return (run_cd(s->words, minishell));
	else if (!ft_strncmp(s->words[0], "pwd", 4))
		printf("%s\n", minishell->pwd);
	else if (!ft_strncmp(s->words[0], "export", 7))
		return (run_export(s->words, minishell));
	else if (!ft_strncmp(s->words[0], "unset", 6))
		return (run_unset(minishell->envp, s));
	else if (!ft_strncmp(s->words[0], "env", 4))
		return (print_env(*minishell->envp, s->words));
	else if (!ft_strncmp(s->words[0], "exit", 5))
		return (run_exit(s, minishell, file));
	return (1);
}

int	only_builtin(t_minishell *minishell, t_put *cmd)
{
	if (!execute_builtin(minishell->ast, minishell, 0, cmd))
	{
		free_heredocs(cmd);
		free(cmd->cmd_fd);
		free(minishell->p.pids);
		if (!ft_strncmp(minishell->ast->words[0], "env", 4))
			return (127);
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
	close_and_free(&minishell->p, cmd, 0);
	if (minishell->p.pids)
		free(minishell->p.pids);
	return (0);
}

void	run_builtin(t_ast *s, t_minishell *minishell, int n, t_put *cmd)
{
	int	flag;

	flag = 0;
	if (!execute_builtin(s, minishell, n, cmd))
	{
		if (!ft_strncmp(s->words[0], "env", 4))
			flag = 1;
		error(minishell, cmd);
		free_ast(&minishell->ast);
		if (flag)
			exit (127);
		exit(1);
	}
	free_2d_array((void ***)minishell->envp);
	free_ast(&minishell->ast);
	free(minishell->p.pids);
	close_and_free(&minishell->p, cmd, 0);
	free_tokens(&minishell->tokens, &minishell->capacity);
	minishell->tokens = NULL;
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
