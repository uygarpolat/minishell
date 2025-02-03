/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_checks.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 12:11:42 by hpirkola          #+#    #+#             */
/*   Updated: 2025/02/03 11:25:20 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"

void	exit_heredocs(t_put *cmd)
{
	free_heredocs(cmd);
	free(cmd->cmd_fd);
}

static void	error_not_path(char *path, t_ast *s, \
		t_minishell *minishell, t_put *cmd)
{
	struct stat	buf;

	if (!path)
	{
		if (!ft_strchr(s->words[0], '/') && get_var(*minishell->envp, "PATH="))
		{
			error(minishell, cmd);
			if (!ft_strncmp(s->words[0], "~", 2))
				print_and_exit(s->words[0], "Is a directory\n", 126, minishell);
			print_and_exit(s->words[0], "command not found\n", 127, minishell);
		}
		else if (stat(s->words[0], &buf) == 0 && access(s->words[0], X_OK) != 0)
		{
			error(minishell, cmd);
			print_and_exit(s->words[0], "Permission denied\n", 126, minishell);
		}
		else if (ft_strchr(s->words[0], '/') || \
			!get_var(*minishell->envp, "PATH="))
		{
			error(minishell, cmd);
			print_and_exit(s->words[0],
				"No such file or directory\n", 127, minishell);
		}
	}
}

static void	stat_zero(char *path, t_ast *s, t_minishell *minishell, t_put *cmd)
{
	if (!ft_strncmp(s->words[0], "..", 3) || !ft_strncmp(s->words[0], "\0", 2))
	{
		free(minishell->p.pids);
		if (minishell->p.pipes)
			close_and_free(&minishell->p, cmd, 0);
		else
			exit_heredocs(cmd);
		free_void((void **)&path, NULL);
		if (get_var(*minishell->envp, "PATH="))
			print_and_exit(s->words[0], "command not found\n", 127, minishell);
		print_and_exit(s->words[0], "Is a directory\n", 126, minishell);
	}
	if (!ft_strncmp(s->words[0], ".", 2) && !s->words[1])
	{
		free_2d_array((void ***)minishell->envp);
		free(minishell->p.pids);
		if (minishell->p.pipes)
			close_and_free(&minishell->p, cmd, 0);
		else
			exit_heredocs(cmd);
		free_void((void **)&path, NULL);
		ft_putstr_fd("minishell: ", 2);
		print_and_exit(s->words[0], \
			"filename argument required\n", 2, minishell);
	}
}

static void	handle_exception(t_minishell *minishell, t_put *cmd)
{
	free_2d_array((void ***)minishell->envp);
	free(minishell->p.pids);
	if (minishell->p.pipes)
		close_and_free(&minishell->p, cmd, 0);
	else
		exit_heredocs(cmd);
}

void	error_check(char *path, t_ast *s, t_minishell *minishell, t_put *cmd)
{
	struct stat	buf;

	stat_zero(path, s, minishell, cmd);
	error_not_path(path, s, minishell, cmd);
	if (stat(path, &buf) == 0)
	{
		if (S_ISDIR(buf.st_mode))
		{
			handle_exception(minishell, cmd);
			if (ft_strncmp(s->words[0], path, ft_strlen(path) + 1))
				free_void((void **)&path, NULL);
			print_and_exit(s->words[0], "Is a directory\n", 126, minishell);
		}
		if (access(path, X_OK) != 0)
		{
			handle_exception(minishell, cmd);
			free_void((void **)&path, NULL);
			print_and_exit(s->words[0], "Permission denied\n", 127, minishell);
		}
	}
}
