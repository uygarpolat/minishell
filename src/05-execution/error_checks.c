/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_checks.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 12:11:42 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/22 19:41:10 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"

void	error_not_path(char *path, t_ast *s, t_minishell *minishell, t_put *cmd)
{
	struct stat	buf;

	if (!path)
	{
		if (!ft_strchr(s->words[0], '/'))
		{
			error(minishell, cmd);
			print_and_exit(s->words[0], "command not found\n", 127, minishell);
		}
		else if (stat(s->words[0], &buf) == 0 && access(s->words[0], X_OK) != 0)
		{
			error(minishell, cmd);
			print_and_exit(s->words[0], "Permission denied\n", 126, minishell);
		}
		else if (ft_strchr(s->words[0], '/'))
		{
			error(minishell, cmd);
			print_and_exit(s->words[0],
				"No such file or directory\n", 127, minishell);
		}
	}
}

void	stat_zero(char *path, t_ast *s, t_minishell *minishell, t_put *cmd)
{
	if (!ft_strncmp(s->words[0], "..", 3) || !ft_strncmp(s->words[0], "\0", 2))
	{
		free_2d_array((void ***)minishell->envp);
		free(minishell->p.pids);
		if (minishell->p.pipes)
			close_and_free(&minishell->p, cmd);
		if (path)
			free(path);
		print_and_exit(s->words[0], "command not found\n", 127, minishell);
	}
	if (!ft_strncmp(s->words[0], ".", 2))
	{
		free_2d_array((void ***)minishell->envp);
		free(minishell->p.pids);
		if (minishell->p.pipes)
			close_and_free(&minishell->p, cmd);
		if (path)
			free(path);
		ft_putstr_fd("minishell: ", 2);
		print_and_exit(s->words[0],
			"filename argument required\n", 2, minishell);
	}
}

void	error_check(char *path, t_ast *s, t_minishell *minishell, t_put *cmd)
{
	struct stat	buf;

	error_not_path(path, s, minishell, cmd);
	if (stat(path, &buf) == 0)
	{
		stat_zero(path, s, minishell, cmd);
		if (S_ISDIR(buf.st_mode))
		{
			free_2d_array((void ***)minishell->envp);
			free(minishell->p.pids);
			if (minishell->p.pipes)
				close_and_free(&minishell->p, cmd);
			print_and_exit(s->words[0], "Is a directory\n", 126, minishell);
		}
		if (access(path, X_OK) != 0)
		{
			free_2d_array((void ***)minishell->envp);
			free(minishell->p.pids);
			if (minishell->p.pipes)
				close_and_free(&minishell->p, cmd);
			if (path)
				free(path);
			print_and_exit(s->words[0], "Permission denied\n", 127, minishell);
		}
	}
}
