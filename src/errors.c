/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 11:05:16 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/20 13:02:55 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"

void	error(t_minishell *minishell, t_put *cmd)
{
	free_2d_array((void ***)minishell->envp);
	if (minishell->p.pipes)
		close_and_free(&minishell->p, cmd);
	if (minishell->p.pids)
		free(minishell->p.pids);
	//if (minishell->ast)
	 	//free_ast(&minishell->ast);
	if (minishell->tokens)
		free_tokens(&minishell->tokens, &minishell->capacity);
}

void	file_error(t_minishell *minishell, t_put *cmd)
{
	free_2d_array((void ***)minishell->envp);
	if (minishell->p.pipes)
		close_and_free(&minishell->p, cmd);
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
		close_and_free(&minishell->p, cmd);
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

void	error_check(char *path, t_ast *s, t_minishell *minishell, t_put *cmd)
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
			print_and_exit(s->words[0], "No such file or directory\n", 127, minishell);
		}
	}
	if (stat(path, &buf) == 0)
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
			print_and_exit(s->words[0], "filename argument required\n", 2, minishell);
		}
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
